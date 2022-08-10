#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestNDTree.py
##! @brief     Test ...
##!
##! @homepage  ###HOMEPAGE###
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import pyohkl as ohkl
import numpy as np
import unittest

class TestNDTRee(unittest.TestCase):
    def test(self):
        # todo(Jonathan) take a close look here!!

        #todo : cant import 'using Ellipsoid3D = Ellipsoid<double, 3>'
        tree = np.array([(0,0,0),(100,100,100)])
        vects = np.array([])
        radius =0.45
        vals = np.array([radius,radius,radius])
        center = np.array([0,0,0])

        # todo : not successfully converted-'std::set<const IShape<double, 3>*> test_set'
        # todo : not able to convert "std::vector<Ellipsoid3D*> shapes;"

        # Lattice of non-intersecting spheres
        i = 1
        j = 1
        k = 1
        for i in range(1,20):
            for j in range(1,20):
                for k in range(1,20):
                    center = np.array([i,j,k])
                    # todo : Ellipsoid3D is not defined
                    # shape = Ellipsoid3D(center,vals,vects)
                    # shape.emplace_back(shape)
                    # tree.addData(shape)
                    k=k+1
                j=j+1
            i = i+1

        # check that the data was inserted correctly

        numChambers = 0
        for chamber in tree:
            numChambers = numChambers + 1
            #for shape in chamber. getData():
                # test_set.insert(shape)

        # todo: tree.numChambers() dont work
        # self.assertAlmostEqual([numChambers,tree.numChambers()])

        # todo : check why 'test_set.size()' doesn't work
        # self.assertAlmostEqual(test_set.size(),19*19*19)

        # todo : tree.getCollsions().size() doesn't work
        # check that they dont intersect
        # self.assertAlmostEqual([tree.getCollisions().size(),0])

        # add some spheres which will intersect
        center = np.array([1.5,1.5,1.5])
        vals = np.array([ radius,radius,radius ])
        # ellipsoid3D is not defined
        # shape = Ellipsoid3D(center, vals, vects)

        # self.assertAlmostEqual(tree.getCollisions(Shape).size(),8)

        # todo : what to do with 'shape = nullptr'

        # clear the list
        # todo : shapes is not defined
        # for shape in shapes():
         #   del shape

    def split_test(self):
        Ellipsoid3D = ohkl.Ellipsoid()
        tree = np.array([(0,0,0),(50,50,50)])
        tree.setMaxStorage(4)

        vects = np.array([])
        radius = 1.0
        vals = np.array([radius,radius,radius])
        test_set = IShape()
        shapes = Ellipsoid3D()

        shapes.emplace_back(Ellipsoid3D(np.array([(12.5,12.5,12.5),vals,vects])))
        shapes.emplace_back(Ellipsoid3D(np.array([(12.5,12.5,37.5),vals,vects])))
        shapes.emplace_back(Ellipsoid3D(np.array([(12.5,37.5,12.5),vals,vects])))
        shapes.emplace_back(Ellipsoid3D(np.array([(12.5,37.5,37.5),vals,vects])))
        shapes.emplace_back(Ellipsoid3D(np.array([(37.5,12.5,12.5),vals,vects])))
        shapes.emplace_back(Ellipsoid3D(np.array([(37.5,12.5,37.5),vals,vects])))
        shapes.emplace_back(Ellipsoid3D(np.array([(37.5,37.5,12.5),vals,vects])))
        shapes.emplace_back(Ellipsoid3D(np.array([(37.5,37.5,37.5),vals,vects])))

        for shape in shapes():
            tree.addData(shape)

        # check that it split properly
        self.assertAlmostEqual([tree.numChambers(),8])

        # check that the collisions with chambers make sense
        for shape in shapes:
            num_intercept = 0
            for chamber in tree:
                self.assertAlmostEqual([shape.intercept(chamber),chamber.intercept(shape)])
                if shape.intercept(chamber):
                    num_intercept = num_intercept + 1

            self.assertAlmostEqual([num_intercept,1])


        # clear the list
        for shape in shapes:
            del shape

    def test_case(Test_NDTree):
        # build up a NDTree with (0,0,0) as lower corner and (100,100,100) as upper corner
        tree = np.array([(0,0,0),(100,100,100)])
        # todo : the same line "tree.setMaxStrorage(4)" works in one method but not in other?
        # tree.setMaxStorage(10)

        np.random.uniform(0,50)
        gen = np.random

        bb = np.array([0.0,0.0,0.0])
        maxStorage = 10

        i = 0
        for i in range(0,maxStorage):
            # v1 = np.array([d1(gen),d1(gen),d1(gen)])
            # v2 = np.array([d2(gen),d2(gen),d2(gen)])
            pass

if __name__ == '__main__':
    unittest.main()
