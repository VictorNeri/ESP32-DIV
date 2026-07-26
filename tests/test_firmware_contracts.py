import pathlib
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]


class FirmwareContractTests(unittest.TestCase):
    def test_unified_resistive_menu_contract(self):
        logic = (ROOT / "menu_navigation_logic.h").read_text(encoding="utf-8")
        ui_path = ROOT / "menu_navigation_ui.cpp"
        self.assertTrue(ui_path.is_file())
        self.assertTrue((ROOT / "menu_navigation_ui.h").is_file())
        ui = ui_path.read_text(encoding="utf-8")
        self.assertIn("ROWS_PER_PAGE = 5", logic)
        self.assertIn("ROW_HEIGHT = 44", logic)
        self.assertIn("ReleaseTracker", logic)
        self.assertIn("drawMenu", ui)
        self.assertIn('"< BACK"', ui)
        self.assertIn('"PREV"', ui)
        self.assertIn('"NEXT"', ui)

    def test_main_and_legacy_submenus_use_unified_navigation(self):
        sketch = (ROOT / "quetzal.ino").read_text(encoding="utf-8")
        self.assertIn('#include "menu_navigation_ui.h"', sketch)
        self.assertIn("handleUnifiedMenuTouch", sketch)
        self.assertIn("MenuNavigation::ReleaseTracker", sketch)
        self.assertGreaterEqual(sketch.count("UnifiedMenu::drawMenu"), 2)
        self.assertIn("injectedButton", sketch)
        self.assertNotIn("while (ts.touched() && (millis() - startTime < touchFeedbackDelay))", sketch)

    def test_espnow_replaces_ir_remote_menu(self):
        sketch = (ROOT / "quetzal.ino").read_text(encoding="utf-8")
        self.assertNotIn('"IR Remote"', sketch)
        self.assertIn('"ESP-NOW"', sketch)
        self.assertIn('#include "espnow_test.h"', sketch)
        self.assertTrue((ROOT / "espnow_test.cpp").is_file())
        self.assertTrue((ROOT / "espnow_test.h").is_file())
        self.assertIn("EspNowTest::espNowTestSetup", sketch)
        self.assertIn("EspNowTest::espNowTestCleanup", sketch)

    def test_host_scanner_uses_real_subnet_and_bounded_icmp_probe(self):
        source = (ROOT / "host_scanner.cpp").read_text(encoding="utf-8")
        self.assertIn("WiFi.subnetMask()", source)
        self.assertIn("SOCK_RAW", source)
        self.assertIn("IPPROTO_ICMP", source)
        self.assertIn("close(pingSocket)", source)
        self.assertNotIn("esp_ping_new_session", source)
        self.assertNotIn("client.connect(target, 80, CONNECT_TIMEOUT_MS)", source)
        self.assertIn("networkAddress", source)
        self.assertIn("broadcastAddress", source)

    def test_host_scanner_results_support_paging_and_detail(self):
        source = (ROOT / "host_scanner.cpp").read_text(encoding="utf-8")
        self.assertIn("RESULTS_PER_PAGE", source)
        self.assertIn("listStartIndex", source)
        self.assertIn("scrollResults", source)
        self.assertIn("drawResultDetail", source)
        self.assertIn("resultsTruncated", source)
        self.assertIn("if (!aborted) resultCount++", source)

    def test_espnow_reports_full_v2_frame_length(self):
        source = (ROOT / "espnow_test.cpp").read_text(encoding="utf-8")
        self.assertIn("volatile uint16_t lastLength", source)
        self.assertNotIn("min(length, 255)", source)
    def test_wifi_assessment_suite_contract(self):
        sketch = (ROOT / "quetzal.ino").read_text(encoding="utf-8")
        parser = (ROOT / "wifi_80211.cpp").read_text(encoding="utf-8")
        capture = (ROOT / "wifi_capture.cpp").read_text(encoding="utf-8")
        suite = (ROOT / "wifi_assessment.cpp").read_text(encoding="utf-8")
        self.assertIn('"Assessment Suite"', sketch)
        for name in (
            "Config Auditor", "EAPOL Capture", "Mgmt Analyzer",
            "Rogue AP Detector", "AP/Client Mapper", "WPS Scanner",
            "Channel Survey", "Deauth Resilience",
        ):
            self.assertIn(name, suite)
        self.assertIn("parseSecurityIes", parser)
        self.assertIn("findEapol", parser)
        self.assertIn("DLT_IEEE802_11_RADIO", suite)
        self.assertIn("esp_wifi_set_promiscuous_rx_cb", capture)
        self.assertIn("hasInconsistentDuplicate", suite)
        self.assertIn("MAX_DEAUTH_FRAMES", suite)
        self.assertIn("AUTHORIZATION_REQUIRED", suite)

    def test_wifi_assessment_uses_unified_menu_navigation(self):
        suite = (ROOT / "wifi_assessment.cpp").read_text(encoding="utf-8")
        self.assertIn('#include "menu_navigation_ui.h"', suite)
        self.assertIn("MenuNavigation::ReleaseTracker", suite)
        self.assertIn("MenuNavigation::hitTestMenu", suite)
        self.assertIn("UnifiedMenu::drawMenu", suite)
        self.assertNotIn("i * 27", suite)
        self.assertIn("resilienceStopped", suite)
        self.assertIn("EMERGENCY STOP", suite)
        self.assertIn("fillRect(0, RESILIENCE_ACTION_TOP, SCREEN_W", suite)
        self.assertNotIn("view == View::Resilience && y >= 225", suite)
        self.assertIn("view == View::Resilience && y >= RESILIENCE_ACTION_TOP", suite)

    def test_wifi_assessment_safety_regressions(self):
        suite = (ROOT / "wifi_assessment.cpp").read_text(encoding="utf-8")
        capture = (ROOT / "wifi_capture.cpp").read_text(encoding="utf-8")
        parser = (ROOT / "wifi_80211.cpp").read_text(encoding="utf-8")
        self.assertIn("esp_wifi_get_country", suite)
        self.assertIn("ABORTED: target channel unavailable", suite)
        self.assertIn("Baseline not seen: inconclusive", suite)
        self.assertIn("Ignore group-key handshakes", suite)
        self.assertIn("handshakeClient", suite)
        self.assertIn("bodyLength", suite)
        self.assertGreaterEqual(suite.count("clientCount = 0"), 3)
        self.assertIn("if (!captureActive)", capture)
        self.assertIn("record.timestampUs", suite)
        self.assertIn("surveyPackets[record.channel]", suite)
        self.assertIn("toolCaptureError", suite)
        self.assertIn("observationCaptureError", suite)
        self.assertIn("if ((out.subtype & 0x8) != 0)", parser)

    def test_ble_assessment_suite_contract(self):
        sketch = (ROOT / "quetzal.ino").read_text(encoding="utf-8")
        suite_path = ROOT / "ble_assessment.cpp"
        logic_path = ROOT / "ble_assessment_logic.cpp"
        self.assertTrue(suite_path.is_file())
        self.assertTrue((ROOT / "ble_assessment.h").is_file())
        self.assertTrue(logic_path.is_file())
        suite = suite_path.read_text(encoding="utf-8")
        self.assertIn('#include "ble_assessment.h"', sketch)
        self.assertIn('"Assessment Suite"', sketch)
        for name in (
            "Security Auditor", "GATT Permissions", "Privacy Analyzer",
            "Pairing Resilience", "Rogue Peripheral", "Notification Monitor",
            "ATT Robustness", "Connection Resilience", "Mesh Auditor",
            "Replay Tester",
        ):
            self.assertIn(name, suite)
        self.assertIn("BleAssessment::setup", sketch)
        self.assertIn("BleAssessment::cleanup", sketch)

    def test_ble_assessment_uses_unified_menu_navigation(self):
        suite = (ROOT / "ble_assessment.cpp").read_text(encoding="utf-8")
        self.assertIn('#include "menu_navigation_ui.h"', suite)
        self.assertIn("MenuNavigation::ReleaseTracker", suite)
        self.assertIn("MenuNavigation::hitTestMenu", suite)
        self.assertIn("UnifiedMenu::drawMenu", suite)
        self.assertIn("Select target:", suite)
        self.assertNotIn('"tap bottom to page"', suite)
        self.assertNotIn('"bottom=rescan/page"', suite)
        self.assertNotIn("toolPage * TOOLS_PER_PAGE", suite)
        self.assertNotIn("devicePage * 10", suite)
        self.assertIn("fillRect(0, 270, 240, 50", suite)
        self.assertIn("if (y < 270) return false;", suite)
        self.assertIn("updateStatusBar();", suite)
        self.assertIn("activeStage == ActiveStage::Running && emergencyStop()", suite)

    def test_status_popup_consumes_close_touch_release(self):
        utils = (ROOT / "utils.cpp").read_text(encoding="utf-8")
        self.assertIn("while (ts.touched()) delay(10);", utils)

    def test_ble_assessment_active_safety_contract(self):
        suite = (ROOT / "ble_assessment.cpp").read_text(encoding="utf-8")
        for token in (
            "AUTHORIZATION_REQUIRED", "MAX_CONNECTION_ATTEMPTS",
            "MAX_ATT_REQUESTS", "MAX_REPLAY_BYTES", "emergencyStop",
            "authorized", "confirmed", "selectedDevice",
            "disconnectClient", "stopScan", "unsubscribeAll",
        ):
            self.assertIn(token, suite)
        self.assertNotIn("deleteAllBonds", suite)
        self.assertNotIn("deleteBond(", suite)
        self.assertNotIn("setSecurityAuth", suite)
        self.assertNotIn("readValue(", suite)
        self.assertIn("rawRead.overflow", suite)
        self.assertIn("replayPeerAddress", suite)
        self.assertIn("ble_gattc_read", suite)
        self.assertIn("ble_gattc_read_long", suite)
        self.assertIn("BleHidInject::isConnected()", suite)
        self.assertIn("callbacksEnabled = false", suite)
        self.assertIn("writes are never automatic", suite)
        self.assertIn("notificationPayloadHash", suite)
        self.assertIn("MESH_PROVISIONING_UUID", suite)
        self.assertEqual(suite.count("writeValue("), 1)
        self.assertIn("replayCandidateIndex", suite)
        self.assertIn("replayUuid", suite)
        self.assertIn("MAX_SUBSCRIPTIONS", suite)
        self.assertIn("scan->setMaxResults(MAX_DEVICES)", suite)
        self.assertIn("scan->setActiveScan(false)", suite)
        self.assertIn("targetSelected", suite)
        self.assertIn("authorizedTarget", suite)
        self.assertIn("AssessmentClientCallbacks", suite)
        self.assertIn("activeStage != beforeStage", suite)
        self.assertIn("secureConnection(true)", suite)
        self.assertNotIn("secureConnection(false)", suite)
        self.assertIn("processPairingTest", suite)
        self.assertIn("subscriptionSecurityTransitions", suite)
        self.assertIn("unencryptedNotificationCount", suite)
        notification_start = suite.index("for (size_t i = 0; i < characteristicCount && subscriptionCount")
        notification_end = suite.index("if (subscriptionCount == 0)", notification_start)
        self.assertGreaterEqual(suite[notification_start:notification_end].count("emergencyStop()"), 2)
        replay_start = suite.index("void sendReplayOnce()")
        replay_end = suite.index("void drawHeader", replay_start)
        replay = suite[replay_start:replay_end]
        self.assertIn("drawTool();\n  waitForTouchRelease();", replay)
        self.assertGreaterEqual(replay.count("emergencyStop()"), 2)
        self.assertIn("subscriptionUnencryptedEvents", suite)
        self.assertIn("lastNotificationUuid", suite)
        self.assertIn("notificationCallback(generation, source", suite)
        self.assertIn("getConnHandle() == BLE_HS_CONN_HANDLE_NONE", suite)
        self.assertLess(suite.index("Authorize before capture/read"),
                        suite.index("captureReplayValue(false)", suite.index("void handleActiveAction")))


if __name__ == "__main__":
    unittest.main()
