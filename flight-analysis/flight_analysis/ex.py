import math
import matplotlib.pyplot as plt

fig = plt.figure()
ax = plt.axes(projection='3d')
ax.set_xlim([0, 1])
ax.set_ylim([0, 1])
ax.set_zlim([0, 1])

vector = None
for i in range(1080):
    roll = math.radians(0)
    pitch = math.radians(0)
    yaw = math.radians(i)

    # x = -math.cos(yaw) * math.cos(pitch) * math.sin(roll) - math.sin(yaw) * math.cos(roll)
    # y = -math.sin(yaw) * math.sin(pitch) * math.sin(roll) + math.cos(yaw) * math.cos(roll)
    # z = math.cos(pitch) * math.sin(roll)

    x = math.cos(yaw) * math.cos(pitch)
    y = math.sin(yaw) * math.cos(pitch)
    z = math.sin(pitch)

    if vector:
        vector.remove()

    vector = ax.quiver(0, 0, 0, x, y, z, length=1, normalize=True)

    plt.pause(0.005)
