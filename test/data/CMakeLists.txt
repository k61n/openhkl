include(ExternalData)

configure_file(../cpp/crystal/lattices.dat lattices.dat COPYONLY)
configure_file(../cpp/crystal/niggli_lattices.dat niggli_lattices.dat COPYONLY)
configure_file(../cpp/crystal/CsOsO_15K.raf CsOsO_15K.raf COPYONLY)
configure_file(../cpp/crystal/CsOsO_15K.lpt CsOsO_15K.lpt COPYONLY)

configure_file(../cpp/data/D10_ascii_example D10_ascii_example COPYONLY)
configure_file(../cpp/data/D9_ascii_example D9_ascii_example COPYONLY)
configure_file(../cpp/data/D9_QSCAN D9_QSCAN COPYONLY)

configure_file(../cpp/data/gal3.hdf gal3.hdf COPYONLY)
configure_file(../cpp/data/d19_test.hdf d19_test.hdf COPYONLY)
configure_file(../cpp/data/H5_example.hdf H5_example.hdf COPYONLY)
configure_file(../cpp/data/blank.hdf blank.hdf COPYONLY)

configure_file(../cpp/geometry/convex_hull_faces.xyz convex_hull_faces.xyz COPYONLY)
configure_file(../cpp/geometry/convex_hull_vertices.xyz convex_hull_vertices.xyz COPYONLY)

configure_file(../cpp/instrument/RotAxis.xml RotAxis.xml COPYONLY)

configure_file(../cpp/instrument/D10_ascii_example D10_ascii_example COPYONLY)
configure_file(../cpp/instrument/Instrument.xml Instrument.xml COPYONLY)

configure_file(../cpp/crystal/crystal_database.csv crystal_database.csv COPYONLY)
configure_file(../cpp/crystal/crystallography.tsv crystallography.tsv COPYONLY)
configure_file(../cpp/crystal/714898.hdf 714898.hdf COPYONLY)

configure_file(../python/data/md5/FutA.nsx.md5 md5/FutA.nsx.md5 COPYONLY)
configure_file(../python/data/md5/FutA-indexed.nsx.md5 md5/FutA-indexed.nsx.md5 COPYONLY)
configure_file(../python/data/md5/p16106_00029299.tiff.md5 md5/p16106_00029299.tiff.md5 COPYONLY)
configure_file(../python/data/md5/p16106_00029300.tiff.md5 md5/p16106_00029300.tiff.md5 COPYONLY)
configure_file(../python/data/md5/p16106_00029301.tiff.md5 md5/p16106_00029301.tiff.md5 COPYONLY)
configure_file(../python/data/md5/p16106_00029302.tiff.md5 md5/p16106_00029302.tiff.md5 COPYONLY)
configure_file(../python/data/md5/p16106_00029303.tiff.md5 md5/p16106_00029303.tiff.md5 COPYONLY)
configure_file(../python/data/md5/p16106_00029304.tiff.md5 md5/p16106_00029304.tiff.md5 COPYONLY)
configure_file(../python/data/md5/p16106_00029305.tiff.md5 md5/p16106_00029305.tiff.md5 COPYONLY)

message("start downloading")
set(algo md5)
message("%(algo)")
set(ExternalData_URL_TEMPLATES
    "file:///%(algo)/%(hash)"
    "http://apps.jcns.fz-juelich.de/datasamples/nsxtool/BioDiff/FutA/crystal9/%(hash)"
)
ExternalData_Add_Test(TestAutoIndexerData
	NAME TestAutoIndexer
	COMMAND TestAutoIndexer.py --input DATA{md5/FutA.nsx} -t
)
ExternalData_Add_Target(TestAutoIndexerData)
message("end downloading")
