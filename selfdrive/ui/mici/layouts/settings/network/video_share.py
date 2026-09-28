import threading
import time
from typing import Union

import numpy as np
import pyray as rl
import qrcode

from openpilot.common.params import Params
from openpilot.common.swaglog import cloudlog
from openpilot.system.mediaserver.constants import PORT
from openpilot.system.ui.lib.application import FontWeight
from openpilot.system.ui.lib.wifi_manager import TETHERING_IP_ADDRESS, WifiManager
from openpilot.system.ui.widgets.label import UnifiedLabel
from openpilot.system.ui.widgets.nav_widget import NavWidget

VIDEO_URL = f"http://{TETHERING_IP_ADDRESS}:{PORT}/"


def wifi_qr_payload(ssid: str, password: str) -> str:
  """Wi-Fi network QR code that phone cameras can join"""
  def escape(s: str) -> str:
    for c in '\\;,:"':
      s = s.replace(c, '\\' + c)
    return s
  return f"WIFI:T:WPA;S:{escape(ssid)};P:{escape(password)};;"


class VideoShare:
  """Sharing the recorded videos with a phone: the ShareVideos param runs mediaserverd, and the hotspot lets the phone connect.

  mediaserverd clears ShareVideos when the car starts moving or nothing uses it, then the hotspot is turned off here.
  """
  def __init__(self, wifi_manager: WifiManager):
    self._wifi_manager = wifi_manager
    self._params = Params()
    self._started_tethering = False
    threading.Thread(target=self._watch, daemon=True).start()

  @property
  def enabled(self) -> bool:
    return self._params.get_bool("ShareVideos")

  def start(self):
    # written before the hotspot is turned on, so _watch doesn't see the old value
    self._params.put_bool("ShareVideos", True, block=True)
    if not self._wifi_manager.is_tethering_active():
      self._wifi_manager.set_tethering_active(True)
      self._started_tethering = True

  def stop(self):
    self._params.put_bool("ShareVideos", False)

  def _watch(self):
    while True:
      if self._started_tethering and not self.enabled:
        self._started_tethering = False
        self._wifi_manager.set_tethering_active(False)
      time.sleep(1)


def qr_texture(data: str) -> Union[rl.Texture, None]:
  try:
    qr = qrcode.QRCode(error_correction=qrcode.constants.ERROR_CORRECT_L, box_size=10, border=2)
    qr.add_data(data)
    qr.make(fit=True)
    # dark on light with a quiet zone, which phone cameras read more reliably for Wi-Fi codes
    pil_img = qr.make_image(fill_color="black", back_color="white").convert('RGBA')
    img_array = np.array(pil_img, dtype=np.uint8)

    rl_image = rl.Image()
    rl_image.data = rl.ffi.cast("void *", img_array.ctypes.data)
    rl_image.width = pil_img.width
    rl_image.height = pil_img.height
    rl_image.mipmaps = 1
    rl_image.format = rl.PixelFormat.PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    return rl.load_texture_from_image(rl_image)
  except Exception as e:
    cloudlog.warning(f"QR code generation failed: {e}")
    return None


class VideoShareDialog(NavWidget):
  """Two QR codes: join the device's hotspot, then open the video player"""
  QR_SIZE = 180

  def __init__(self, wifi_manager: WifiManager):
    super().__init__()
    self._wifi_manager = wifi_manager
    self._wifi_payload = ""
    self._wifi_texture: rl.Texture | None = None
    self._url_texture = qr_texture(VIDEO_URL)
    self._wifi_label = UnifiedLabel("1. join wi-fi", font_size=32, font_weight=FontWeight.BOLD)
    self._url_label = UnifiedLabel("2. open videos", font_size=32, font_weight=FontWeight.BOLD)

  def _update_state(self):
    super()._update_state()
    # the hotspot password is read in the background after startup, and can be changed
    payload = wifi_qr_payload(self._wifi_manager.tethering_ssid, self._wifi_manager.tethering_password)
    if payload != self._wifi_payload and self._wifi_manager.tethering_password:
      self._wifi_payload = payload
      self._unload(self._wifi_texture)
      self._wifi_texture = qr_texture(payload)

  def _render(self, rect: rl.Rectangle):
    gap = (self._rect.width - 2 * self.QR_SIZE) / 3
    for i, (texture, label) in enumerate(((self._wifi_texture, self._wifi_label), (self._url_texture, self._url_label))):
      x = self._rect.x + gap + i * (self.QR_SIZE + gap)
      label.set_max_width(int(self.QR_SIZE + gap))
      label.set_position(x, self._rect.y + 4)
      label.render()
      if texture is not None:
        pos = rl.Vector2(round(x), round(self._rect.y + self._rect.height - self.QR_SIZE - 8))
        rl.draw_texture_ex(texture, pos, 0.0, self.QR_SIZE / texture.height, rl.WHITE)

  @staticmethod
  def _unload(texture: Union[rl.Texture, None]):
    if texture is not None and texture.id != 0:
      rl.unload_texture(texture)

  def __del__(self):
    self._unload(self._wifi_texture)
    self._unload(self._url_texture)
