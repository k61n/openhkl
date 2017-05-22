import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestILLAscii(unittest.TestCase):

    def test(self):
        factory = nsx.DataReaderFactory()
        ds = nsx.DiffractometerStore()
        diff = ds.buildDiffractometer("D10")
        dataf = factory.create("", "D10_ascii_example", diff)
        meta = dataf.getMetadata()
        dataf.open()
        v = dataf.getFrame(0)

   # todo : check if the catch methods are doing desired work     
    def catch(self):
        e = std.exception() 
        output = string("caught exception")
    def catch(self):
        op = string("unknown exception while loading data")
    
        # check the total no. of count in the frame 0
        self.assertAlmostEqual(v.sum(),65)
        # check the value of monitor 
        self.assertAlmostEquals(meta.getKey("monitor"),20000)

        # todo : check if the following line is correct 
        states = dataf.getInstumentsStates()
        deg = 3.14/180.0
        self.assertAlmostEquals(states[3].detector.getValues()[0],0.54347000E+05/1000.0*deg)
        self.assertAlmostEquals(states[2].sample.getValues()[0],0.26572000E+05/1000.0*deg)
        self.assertAlmostEquals(states[2].sample.getValues()[1],0.48923233E+02*deg)
        self.assertAlmostEquals(states[2].sample.getValues()[2],-0.48583171E+02*deg)

        st = InstrumentState()
        st = dataf.getInterpolatedState(0.5)
        self.assertAlmostEquals(st.detector.getValues()[0],states[0].detector.getValues()[0])
        self.assertAlmostEquals(st.sample.getValues()[0],states[0].sample.getValues()[0]+0.5*(states[1].sample.getValues()[0]-states[0].sample.getValues()[0]))

        st = InstrumentState()
        st = dataf.getInterpolatedState(2.3)
        self.assertAlmostEquals(st.detector.getValues()[0],states[2].detector.getValues()[0]+0.3*(states[3].detector.getValues()[0]-states[2].detector.getValues()[0]))
        self.assertAlmostEquals(st.sample.getValues()[0],states[2].sample.getValues()[0]+0.3*(states[3].sample.getValues()[0]-states[2].sample.getValues()[0]))


if __name__ == '__main__':
    unittest.main()

  
