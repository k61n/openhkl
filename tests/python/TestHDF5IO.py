import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestHDF5IO(unittest.TestCase):
    def test(self):
        factory = nsx.DataReaderFactory()
        ds = nsx.DiffractometerStore()
        # todo : can't convert std::shared_ptr<Diffractrometer>diff
        # todo : can't convert std::shared_ptr<DataSet>dataf

        frames = np.array([]) 

       # todo : if I write code without 'try' thn it works  
       # try: 
        ds = nsx.DiffractometerStore()
        diff = ds.buildDiffractometer("D10")
        dataf = factory.create("","D10_ascii_example",diff)
        dataf.open()
        

        # todo : not sure I did correct conversion of push_back function
        myList = []
        i = 0
        for i in range(0,dataf.getNFrames()): 
            #frames.push_back(dataf.getFrame(i))
            myList.append(i)
        
        dataf.saveHDF5("D10_hdf5_example.h5")
        dataf.close()
        # todo: dataf.reset() is not working
        # dataf.reset()
            
        # read data back in and check that it agrees!
        dataf = factory.create("h5","D10_hdf5_example.h5",diff)

        # todo : check if the conversion of null pointer is correct
        self.assert_(dataf != 0)
        
        # todo: check if its right: used assertTrue for Boost_Check
        j=0
        for j in range(0,dataf,getNFrames()):
            self.assertTrue(dataf.getframe(j) == frames[j]
       
        dataf.close()

        #todo: can't convert 'exception' statements
        # catch (std::exception& e) {
        # BOOST_FAIL(std::string("saveHDF5() threw exception: ") + e.what());
            # }
        # catch(...) {
        # BOOST_FAIL("saveHDF5() threw unknown exception");
        

if __name__ == '__main__':
    unittest.main()

  
