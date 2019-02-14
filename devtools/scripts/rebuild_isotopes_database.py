import collections
import xml.etree.ElementTree as etree

units = collections.OrderedDict()
units["molar_mass"]        = "g_per_mole"
units["natural_abundance"] = "%"
units["half_life"]         = "year"
units["b_coherent"]        = "fm"
units["b_incoherent"]      = "fm"
units["b_minus"]           = "fm"
units["b_plus"]            = "fm"
units["xs_coherent"]       = "barn"
units["xs_incoherent"]     = "barn"
units["xs_scattering"]     = "barn"
units["xs_absorption"]     = "barn"

types = collections.OrderedDict()
types["symbol"]            = "string"
types["element"]           = "string"
types["n_protons"]         = "int"
types["n_nucleons"]        = "int"
types["nuclear_spin"]      = "double"
types["molar_mass"]        = "double"
types["chemical_state"]    = "string"
types["natural_abundance"] = "double"
types["stable"]            = "bool"
types["half_life"]         = "double"
types["b_coherent"]        = "complex"
types["b_incoherent"]      = "complex"
types["b_minus"]           = "complex"
types["b_plus"]            = "complex"
types["xs_coherent"]       = "double"
types["xs_incoherent"]     = "double"
types["xs_scattering"]     = "double"
types["xs_absorption"]     = "double"

tree = etree.parse("isotopes.xml")
root = tree.getroot()

for node in root.findall("isotope"):

	for tag in types.keys():
		subnode = node.find(tag)
		if subnode is None:
			continue
		subnode.attrib["type"] = types[tag]
		subnode.attrib["unit"] = units.get(tag,"au")

	if node.find("natural_abundance") is None:
		naturalAbundance = etree.Element("natural_abundance") 
		naturalAbundance.attrib["type"] = "double"
		naturalAbundance.attrib["unit"] = "%"
		naturalAbundance.text = "0.0"
		naturalAbundance.tail = "\n    "
		node.insert(7,naturalAbundance)

	nNeutrons = etree.Element("n_neutrons") 
	nNeutrons.attrib["type"] = "int"
	nNeutrons.attrib["unit"] = "au"
	nNeutrons.text = str(int(node.find("n_nucleons").text) - int(node.find("n_protons").text))
	nNeutrons.tail = "\n    "
	node.insert(4,nNeutrons)

	nElectrons = etree.Element("n_electrons") 
	nElectrons.attrib["type"] = "int"
	nElectrons.attrib["unit"] = "au"
	nElectrons.text = node.find("n_protons").text
	nElectrons.tail = "\n    "
	node.insert(5,nElectrons)
	node.remove(node.find("n_nucleons"))
	


tree.write("isotopes_new.xml")
		 
