#python

import k3d
import testing

document = k3d.new_document()

source = k3d.plugin.create("PolyTorus", document)

triangles = k3d.plugin.create("TriangulateFaces", document)
triangles.mesh_selection = k3d.select_all()
document.set_dependency(triangles.get_property("input_mesh"), source.get_property("output_mesh"))

modifier = k3d.plugin.create("PGPRemesh", document)
modifier.use_smooth = False
modifier.steps = 0
modifier.omega = 6
modifier.div = 6
modifier.triangulate = False
document.set_dependency(modifier.get_property("input_mesh"), triangles.get_property("output_mesh"))

#print "source output: " + repr(source.output_mesh)
#print "triangles output: " + repr(triangles.output_mesh)
#print "modifier output: " + repr(modifier.output_mesh)

testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.PGPRemesh.triang", 1)
