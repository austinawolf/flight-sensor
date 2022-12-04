import logging
from rx.subject import BehaviorSubject
from blatann.uuid import Uuid128, Uuid16
from blatann.waitables import Waitable
from flight_recorder.services.imu.imu_service_encoder import decode_response, encode_command
from flight_recorder.services.imu.transport import ResponseWaitable, TransportLayerBase
from flight_recorder.services.imu.types.command import Command
from flight_recorder.services.imu.types.response import Response
from flight_recorder.services.imu.types.state_update import SessionStates, StateUpdate

logger = logging.getLogger(__name__)


# class StateUpdateWaitable(Waitable):
#     def __init__(self, on_update:  BehaviorSubject, state: SessionStates):
#         super().__init__()
#         self.state = state
#         self.disposable = on_update.subscribe(self._on_new_state)
#
#     def _on_new_state(self, state_update: StateUpdate):
#         new_state = state_update.current_state
#         previous_state = state_update.previous_state
#         if self.state == new_state:
#             self.disposable.dispose()
#             self._notify(new_state, previous_state)


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

        self._state_update = BehaviorSubject(StateUpdate(SessionStates.IDLE, SessionStates.IDLE))
        self.data_stream = BehaviorSubject(None)

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

    def stream(self, rate, flags: int, session_time: int) -> Response:
        command = Command.stream(rate, flags, session_time)
        return self._send(command).wait(5)

    # def record(self, rate, flags: int, stream_enable: bool, session_time: int) -> Response:
    #     return self.transport.send_command(Record(rate, flags, stream_enable, session_time)).wait(5)
    #
    # def playback(self) -> Response:
    #     return self.transport.send_command(Playback()).wait(5)
    #
    # def stop(self) -> Response:
    #     return self.transport.send_command(Stop()).wait(5)
    #
    # def calibrate(self) -> Response:
    #     return self.transport.send_command(Calibrate()).wait(5)
    #
    # def on_state_update(self, state: SessionStates) -> Waitable:
    #     last_state_update = self._state_update.value
    #     if state == last_state_update.current_state:
    #         return EmptyWaitable(last_state_update.current_state, last_state_update.previous_state)
    #     return StateUpdateWaitable(self._state_update, state)

