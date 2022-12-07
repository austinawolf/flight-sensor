import os
from flight_analysis.session.session import FlightSession

OUTPUT_DIRECTORY = "recordings"


def main():
    files = os.listdir(OUTPUT_DIRECTORY)
    paths = [os.path.join(OUTPUT_DIRECTORY, basename) for basename in files]
    path = max(paths, key=os.path.getctime)

    session = FlightSession.load(path)
    pass

main()
