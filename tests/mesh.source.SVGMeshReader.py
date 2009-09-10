#python

import testing

setup = testing.setup_mesh_reader_test("SVGMeshReader", "mesh.source.SVGMeshReader.svg")
testing.mesh_comparison_to_reference(setup.document, setup.reader.get_property("output_mesh"), "mesh.source.SVGMeshReader", 1, testing.platform_specific)

