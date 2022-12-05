import logging

from blatann.waitables.waitable import EmptyWaitable
from rx.subject import BehaviorSubject
from blatann.uuid import Uuid128, Uuid16
from blatann.waitables import Waitable
from flight_recorder.services.imu.imu_service_encoder import decode_response, encode_command, decode_notification
from flight_recorder.services.imu.transport import ResponseWaitable, TransportLayerBase
from flight_recorder.services.imu.types import SessionState
from flight_recorder.services.imu.types.command import Command
from flight_recorder.services.imu.types.response import Response

logger = logging.getLogger(__name__)


class StateUpdateWaitable(Waitable):
    def __init__(self, on_update: BehaviorSubject, state: SessionState):
        super().__init__()
        self.state = state
        self.disposable = on_update.subscribe(self._on_new_state)

    def _on_new_state(self, new_state: SessionState):
        if self.state.value == new_state.value:
            self.disposable.dispose()
            self._notify(new_state)


class BleImuService:
    DEVICE_NAME = "Flight Sensor"

    class UUIDs:
        IMU_SERVICE = Uuid128("924418ff-c32c-40ca-b9a9-d9024bf8b003")
        COMMAND_CHARACTERISTIC = Uuid16("1770")  # Uuid128("d9541770-a7c0-4a17-a548-1e0fa159ad01")
        DATA_CHARACTERISTIC = Uuid16("2aff")  # Uuid128("43192aff-8264-41bb-b660-678c8ec91201")

    def __init__(self, transport: TransportLayerBase):
        """
        :type gattc_service: blatann.gatt.gattc.GattcService
        """
        self._transport = transport
        self._transport.on_update.subscribe(self._on_notification)

        self.on_state_update = BehaviorSubject(SessionState.IDLE)
        self.on_sample = BehaviorSubject(None)

    def _on_notification(self, payload: bytes):
        if not payload:
            return

        notification = decode_notification(payload)
        if notification.type == notification.Type.STATE_UPDATE:
            self.on_state_update.on_next(notification.current_state)
        elif notification.type == notification.Type.SAMPLE:
            self.on_sample.on_next(notification.sample)
        else:
            raise ValueError(notification.type)

    def _send(self, command):
        if self._transport is None:
            raise Exception("Service not initialized")

        waitable = ResponseWaitable()

        def on_response(response_payload: bytes):
            response = decode_response(response_payload)
            waitable.on_response(response)
            return

        command_payload = encode_command(command)
        self._transport.send_command(command_payload, on_response)

        return waitable

    def _expect_state(self, state: SessionState) -> Waitable:
        last_state_update: SessionState = self.on_state_update.value
        if state.value == last_state_update.value:
            return EmptyWaitable(last_state_update.value)
        return StateUpdateWaitable(self.on_state_update, state)

    def stream(self, rate, flags: int, session_time: int):
        command = Command.stream(rate, flags, session_time)
        response = self._send(command).wait(5)

        if response.status != 0:
            raise Exception

        self._expect_state(SessionState.STREAMING).wait(5)

    def record(self, rate, flags: int, stream_enable: bool, session_time: int):
        command = Command.record(rate, flags, session_time)
        response = self._send(command).wait(5)

        if response.status != 0:
            raise Exception

        self._expect_state(SessionState.RECORDING).wait(5)

    def playback(self):
        command = Command.playback()
        response = self._send(command).wait(5)

        if response.status != 0:
            raise Exception

        self._expect_state(SessionState.PLAYBACK).wait(5)

    def stop(self):
        command = Command.stop()
        response = self._send(command).wait(5)

        if response.status != 0:
            raise Exception

        self._expect_state(SessionState.IDLE).wait(5)

    def calibrate(self):
        command = Command.calibrate()
        response = self._send(command).wait(5)

        if response.status != 0:
            raise Exception

        self._expect_state(SessionState.CALIBRATING).wait(5)

    def wait_for_idle(self, timeout=5):
        self._expect_state(SessionState.IDLE).wait(timeout)
