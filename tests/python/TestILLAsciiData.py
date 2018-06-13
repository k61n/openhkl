import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestILLAscii(unittest.TestCase):

    # todo: fix MetaDat::getKey() and DataSet::getInstrumentStates()
    def test(self):
        factory = nsx.DataReaderFactory()
        diff = nsx.Diffractometer.build("D10")
        dataf = factory.create("", "D10_ascii_example", diff)
        meta = dataf.metadata()
        dataf.open()
        v = dataf.frame(0)
            
        # check the total no. of count in the frame 0
        self.assertAlmostEqual(v.sum(),65)
        # check the value of monitor 

        monitor = meta.key("monitor")        
        #self.assertAlmostEqual(meta.getKey("monitor"),20000)

        # todo : check if the following line is correct 
        states = dataf.instrumentStates()
        deg = 3.14/180.0

        #print(states)
        #print(type(states))

        #self.assertAlmostEqual(states[3].detector.getValues()[0],0.54347000E+05/1000.0*deg)
        #self.assertAlmostEqual(states[2].sample.getValues()[0],0.26572000E+05/1000.0*deg)
        #self.assertAlmostEqual(states[2].sample.getValues()[1],0.48923233E+02*deg)
        #self.assertAlmostEqual(states[2].sample.getValues()[2],-0.48583171E+02*deg)

        st = nsx.InstrumentState()
        st = dataf.interpolatedState(0.5)
        #self.assertAlmostEqual(st.detector.getValues()[0],states[0].detector.getValues()[0])
        #self.assertAlmostEqual(st.sample.getValues()[0],states[0].sample.getValues()[0]+0.5*(states[1].sample.getValues()[0]-states[0].sample.getValues()[0]))

        st = nsx.InstrumentState()
        st = dataf.interpolatedState(2.3)
        #self.assertAlmostEqual(st.detector.getValues()[0],states[2].detector.getValues()[0]+0.3*(states[3].detector.getValues()[0]-states[2].detector.getValues()[0]))
        #self.assertAlmostEqual(st.sample.getValues()[0],states[2].sample.getValues()[0]+0.3*(states[3].sample.getValues()[0]-states[2].sample.getValues()[0]))


if __name__ == '__main__':
    unittest.main()

  
