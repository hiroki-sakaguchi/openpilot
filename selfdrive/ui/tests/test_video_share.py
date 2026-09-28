import time

from openpilot.common.params import Params
from openpilot.selfdrive.ui.mici.layouts.settings.network.video_share import VIDEO_URL, VideoShare, wifi_qr_payload


class FakeWifiManager:
  def __init__(self, tethering=False):
    self.tethering = tethering

  def is_tethering_active(self) -> bool:
    return self.tethering

  def set_tethering_active(self, active: bool):
    self.tethering = active


def wait_for(condition, timeout=5.0) -> bool:
  end = time.monotonic() + timeout
  while time.monotonic() < end:
    if condition():
      return True
    time.sleep(0.1)
  return False


def test_wifi_qr_payload():
  assert wifi_qr_payload("weedle-1234", "swagswagcomma") == "WIFI:T:WPA;S:weedle-1234;P:swagswagcomma;;"
  assert wifi_qr_payload('a;b', 'p:w,"x\\') == 'WIFI:T:WPA;S:a\\;b;P:p\\:w\\,\\"x\\\\;;'


def test_video_url():
  assert VIDEO_URL == "http://192.168.43.1:8088/"


def test_hotspot_follows_share_videos():
  params = Params()
  wifi = FakeWifiManager()
  share = VideoShare(wifi)

  share.start()
  assert params.get_bool("ShareVideos")
  assert wifi.tethering

  # mediaserverd clears the param when it stops, then the hotspot is turned off
  params.put_bool("ShareVideos", False, block=True)
  assert wait_for(lambda: not wifi.tethering)


def test_keeps_hotspot_turned_on_by_user():
  params = Params()
  wifi = FakeWifiManager(tethering=True)
  share = VideoShare(wifi)

  share.start()
  params.put_bool("ShareVideos", False, block=True)
  time.sleep(2.5)
  assert wifi.tethering
