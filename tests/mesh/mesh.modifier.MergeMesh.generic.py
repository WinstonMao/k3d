#python

import k3d
import testing

document = k3d.new_document()

source1= k3d.plugin.create("PolyCube", document)
source1.width = 2.0
source1.height = 2.0
source1.depth = 2.0

source2 = k3d.plugin.create("PolyCube", document)
source2.width = 4.0
source2.height = 4.0
source2.depth = 4.0

modifier1 = k3d.plugin.create("PointsToParticles", document)
modifier1.width = 2.0

modifier2 = k3d.plugin.create("PointsToParticles", document)
modifier2.width = 3.0

document.set_dependency(modifier1.get_property("input_mesh"), source1.get_property("output_mesh"))
document.set_dependency(modifier2.get_property("input_mesh"), source2.get_property("output_mesh"))

merge_mesh = k3d.plugin.create("MergeMesh", document)
merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

document.set_dependency(merge_mesh.get_property("input_mesh1"), modifier1.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh2"), modifier2.get_property("output_mesh"))

testing.require_valid_mesh(document, merge_mesh.get_property("output_mesh"))
testing.require_similar_mesh(document, merge_mesh.get_property("output_mesh"), "mesh.modifier.MergeMesh.generic", 2)
