# 2026-05-24T15:38:28.907114554
import vitis

client = vitis.create_client()
client.set_workspace(path="CPU")

platform = client.get_component(name="platform")
status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../FPGA/platform/ps_wrapper.xsa")

status = platform.build()

comp = client.get_component(name="hello_world")
comp.build()

vitis.dispose()

