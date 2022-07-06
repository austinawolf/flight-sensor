"""
This example demonstrates reading a peripheral's Device Info Service using blatann's device_info service module.
The operations here are programmed in a procedural manner.

This can be used alongside any peripheral which implements the DIS and advertises the 16-bit DIS service UUID.
The peripheral_device_info_service example can be used with this.
"""
import csv
import time

from blatann import BleDevice
from blatann.examples import example_utils
from blatann.nrf import nrf_events
from blatann.uuid import Uuid128
from motion_service import MotionClient

logger = example_utils.setup_logger(level="INFO")

BLE_MOTION_CHAR = Uuid128("43192aff-8264-41bb-b660-678c8ec91201")


data = []


def find_target_device(ble_device, name):
    """
    Starts the scanner and searches the advertising report for the desired name.
    If found, returns the peer's address that can be connected to

    :param ble_device: The ble device to operate on
    :type ble_device: blatann.BleDevice
    :param name: The device's local name that is advertised
    :return: The peer's address if found, or None if not found
    """
    # Start scanning for the peripheral.
    # Using the `scan_reports` iterable on the waitable will return the scan reports as they're
    # discovered in real-time instead of waiting for the full scan to complete
    for report in ble_device.scanner.start_scan().scan_reports:
        if report.advertise_data.local_name == name:
            return report.peer_address


def find_motion_service(gattc_database) -> MotionClient:
    """
    Finds a battery service in the given GATT client database
    :param gattc_database: the GATT client database to search
    :type gattc_database: blatann.gatt.gattc.GattcDatabase
    :return: The Battery service if found, None if not found
    :rtype: _BatteryClient
    """
    return MotionClient.find_in_database(gattc_database)


def write_to_csv(data_, filename):
    with open(filename, 'w', encoding='UTF8', newline='') as f:
        writer = csv.writer(f)

        for d in data_:
            if d.has_quat:
                writer.writerow([d.sample_number, "QUAT", d.quat[0], d.quat[1], d.quat[2], d.quat[3]])
            if d.has_accel:
                writer.writerow([d.sample_number, "ACCEL", d.accel[0], d.accel[1], d.accel[2]])
            if d.has_gyro:
                writer.writerow([d.sample_number, "GYRO", d.gyro[0], d.gyro[1], d.gyro[2]])
            if d.has_compass:
                writer.writerow([d.sample_number, "COMPASS", d.compass[0], d.compass[1], d.compass[2]])


def on_motion_data(_, motion_data):
    data.append(motion_data)


def main(serial_port):
    # Set the target to the peripheral's advertised name
    target_device_name = "FlightSensor"

    # Create and open the BLE device (and suppress spammy logs)
    ble_device = BleDevice(serial_port)
    ble_device.event_logger.suppress(nrf_events.GapEvtAdvReport)
    ble_device.open()

    # Set the scanner to scan for 4 seconds
    ble_device.scanner.set_default_scan_params(timeout_seconds=10)

    logger.info("Scanning for '{}'".format(target_device_name))
    target_address = find_target_device(ble_device, target_device_name)

    if not target_address:
        logger.info("Did not find target peripheral")
        return

    # Initiate the connection and wait for it to finish
    logger.info("Found match: connecting to address {}".format(target_address))
    peer = ble_device.connect(target_address).wait()
    if not peer:
        logger.warning("Timed out connecting to device")
        return
    logger.info("Connected, conn_handle: {}".format(peer.conn_handle))

    # Wait up to 10 seconds for service discovery to complete
    _, event_args = peer.discover_services().wait(10, exception_on_timeout=False)
    logger.info("Service discovery complete! status: {}".format(event_args.status))

    # Find service
    motion_service = find_motion_service(peer.database)
    if not motion_service:
        logger.info("Failed to find Motion Service in peripheral database")
        peer.disconnect().wait()
        return

    # On Notification Received
    motion_service.on_motion_data_received.register(on_motion_data)
    motion_service.enable_notifications().wait()

    filename = input("Filename: ")

    # Clean up
    logger.info("Disconnecting from peripheral")
    if peer:
        peer.disconnect().wait()
    ble_device.close()

    # Write to file
    write_to_csv(data, filename)


if __name__ == '__main__':
    main("COM14")
