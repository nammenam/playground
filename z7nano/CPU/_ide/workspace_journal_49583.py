# 2026-05-22T20:46:21.714784060
import vitis

client = vitis.create_client()
client.set_workspace(path="CPU")

platform = client.create_platform_component(name = "platform",hw_design = "$COMPONENT_LOCATION/../../FPGA/platform/ps_wrapper.xsa",os = "standalone",cpu = "ps7_cortexa9_0",domain_name = "standalone_ps7_cortexa9_0",compiler = "gcc")

platform = client.get_component(name="platform")
status = platform.build()

comp = client.create_app_component(name="hello_world",platform = "$COMPONENT_LOCATION/../platform/export/platform/platform.xpfm",domain = "standalone_ps7_cortexa9_0",template = "hello_world")

comp = client.get_component(name="hello_world")
comp.build()

comp.build()

vitis.dispose()

