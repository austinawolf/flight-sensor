import binascii
import datetime
import logging
from typing import List

from blatann.waitables.waitable import EmptyWaitable
from rx.subject import BehaviorSubject
from blatann.event_type import EventSource
from blatann.services.ble_data_types import BleDataStream
from blatann.uuid import Uuid128, Uuid16
from blatann.waitables import Waitable
from flight_recorder.packets.commands import Stream, Record, Playback, Stop, Calibrate
from flight_recorder.packets.data import Data
from flight_recorder.packets.responses import Response
from flight_recorder.packets.state_update import SessionStates, StateUpdate
from flight_recorder.services.ble_imu.session import BleImuSession
from flight_recorder.services.ble_imu.transport import BleImuTransportLayer

logger = logging.getLogger(__name__)


class StateUpdateWaitable(Waitable):
    def __init__(self, on_update:  BehaviorSubject, state: SessionStates):
        super().__init__()
        self.state = state
        self.disposable = on_update.subscribe(self._on_new_state)

    def _on_new_state(self, state_update: StateUpdate):
        new_state = state_update.current_state
        previous_state = state_update.previous_state
        if self.state == new_state:
            self.disposable.dispose()
            self._notify(new_state, previous_state)


class BleImuService:
    DEVICE_NAME = "Flight Sensor"

    class UUIDs:
        IMU_SERVICE = Uuid128("924418ff-c32c-40ca-b9a9-d9024bf8b003")
        COMMAND_CHARACTERISTIC = Uuid16("1770")  # Uuid128("d9541770-a7c0-4a17-a548-1e0fa159ad01")
        DATA_CHARACTERISTIC = Uuid16("2aff")  # Uuid128("43192aff-8264-41bb-b660-678c8ec91201")

    def __init__(self):
        """
        :type gattc_service: blatann.gatt.gattc.GattcService
        """
        self._service = None
        self._command_characteristic = None
        self._data_characteristic = None
        self._data = []
        self._state_update = BehaviorSubject(StateUpdate(SessionStates.IDLE, SessionStates.IDLE))
        self.transport = BleImuTransportLayer(self._transmit, self._on_data, self._on_state_update)

    def _on_notification(self, char, event_args):
        self.transport.on_receive(BleDataStream(event_args.value))

    def _transmit(self, stream: BleDataStream):
        self._command_characteristic.write(stream)

    def _on_data(self, data: Data):
        self._data.append(data)

    def _on_state_update(self, state_update: StateUpdate):
        logger.info(f"State Update: {state_update.previous_state.name} -> {state_update.current_state.name}")
        self._state_update.on_next(state_update)

    def initialize(self, peer):
        # Find Characteristics
        self._service = peer.database.find_service(self.UUIDs.IMU_SERVICE)
        self._command_characteristic = peer.database.find_characteristic(self.UUIDs.COMMAND_CHARACTERISTIC)
        self._data_characteristic = peer.database.find_characteristic(self.UUIDs.DATA_CHARACTERISTIC)

        self._command_characteristic.subscribe(self._on_notification)
        self._data_characteristic.subscribe(self._on_notification)

    def stream(self, rate, flags: int, session_time: int) -> Response:
        return self.transport.send_command(Stream(rate, flags, session_time)).wait(5)

    def record(self, rate, flags: int, stream_enable: bool, session_time: int) -> Response:
        return self.transport.send_command(Record(rate, flags, stream_enable, session_time)).wait(5)

    def playback(self) -> Response:
        return self.transport.send_command(Playback()).wait(5)

    def stop(self) -> Response:
        return self.transport.send_command(Stop()).wait(5)

    def calibrate(self) -> Response:
        return self.transport.send_command(Calibrate()).wait(5)

    def on_state_update(self, state: SessionStates) -> Waitable:
        last_state_update = self._state_update.value
        if state == last_state_update.current_state:
            return EmptyWaitable(last_state_update.current_state, last_state_update.previous_state)
        return StateUpdateWaitable(self._state_update, state)

    def save_report(self, filename: str):
        session = BleImuSession(datetime.datetime.now(), 0, 0, self._data)
        session.save_report(filename)
