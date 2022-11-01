import pyvista as pv
from pyvista import examples

p = pv.Plotter()

axes = pv.Axes(show_actor=True, actor_scale=2.0, line_width=5)
axes.origin = (0.0, 0.0, 0.0)


x = 180
y = 180
z = 180


def display():
    global x
    global y
    global z
    obj = pv.Arrow()
    obj = obj.rotate_x(x, point=axes.origin, inplace=False)
    obj = obj.rotate_y(y, point=axes.origin, inplace=False)
    obj = obj.rotate_z(z, point=axes.origin, inplace=False)
    p.add_mesh(obj, name='Object', show_edges=True)


def on_new_x(value):
    global x
    x = value
    display()
    return


def on_new_y(value):
    global y
    y = value
    display()
    return


def on_new_z(value):
    global z
    z = value
    display()
    return


p.add_slider_widget(on_new_x, [0, 360], title='X', pointa=(0.4, 0.9), pointb=(0.9, 0.9))
p.add_slider_widget(on_new_y, [0, 360], title='Y', pointa=(0.4, 0.7), pointb=(0.9, 0.7))
p.add_slider_widget(on_new_z, [0, 360], title='Z', pointa=(0.4, 0.5), pointb=(0.9, 0.5))

p.show()
