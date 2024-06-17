import unittest
import os

import source_discovery

class TestCacheInvalidation(unittest.TestCase):
    
    @classmethod
    def setUpClass(this):
        this.selfpath = os.path.dirname(os.path.realpath(__file__))

        emptyDir = os.path.join( this.selfpath, "test_data", "empty")
        if not os.path.exists(emptyDir): os.mkdir(emptyDir)
            
        os.chdir(emptyDir                                             ) ; this.data_empty   = source_discovery.discoverAll([ "." ])
        os.chdir(os.path.join( this.selfpath, "test_data", "simple1" )) ; this.data_simple1 = source_discovery.discoverAll([ "." ])
        os.chdir(os.path.join( this.selfpath, "test_data", "simple2" )) ; this.data_simple2 = source_discovery.discoverAll([ "." ])
        os.chdir(os.path.join( this.selfpath, "test_data", "includes1" )) ; this.data_includes1 = source_discovery.discoverAll([ "." ])
        os.chdir(os.path.join( this.selfpath, "test_data", "includes2" )) ; this.data_includes2 = source_discovery.discoverAll([ "." ])
        os.chdir(os.path.join( this.selfpath, "test_data", "includes3" )) ; this.data_includes3 = source_discovery.discoverAll([ "." ])
        os.chdir(this.selfpath)
        assert this.data_simple1[0].contents != this.data_simple2[0].contents

    def test_added(this):
        diff = source_discovery.ProjectDiff(this.data_empty, this.data_simple2)
        this.assertEqual(len(diff.new     ), 1, "Added file: False negative")
        this.assertEqual(len(diff.removed ), 0, "Added file: False positive")
        this.assertEqual(len(diff.outdated), 0, "Added file: False positive")
        this.assertEqual(len(diff.upToDate), 0, "Added file: False positive")
    
    def test_deleted(this):
        diff = source_discovery.ProjectDiff(this.data_simple1, this.data_empty)
        this.assertEqual(len(diff.new     ), 0, "Deleted file: False positive")
        this.assertEqual(len(diff.removed ), 1, "Deleted file: False negative")
        this.assertEqual(len(diff.outdated), 0, "Deleted file: False positive")
        this.assertEqual(len(diff.upToDate), 0, "Deleted file: False positive")
    
    def test_edited(this):
        diff = source_discovery.ProjectDiff(this.data_simple1, this.data_simple2)
        this.assertEqual(len(diff.new     ), 0, "Edited file: False positive")
        this.assertEqual(len(diff.removed ), 0, "Edited file: False positive")
        this.assertEqual(len(diff.outdated), 1, "Edited file: False negative")
        this.assertEqual(len(diff.upToDate), 0, "Edited file: False positive")
    
    def test_same(this):
        diff = source_discovery.ProjectDiff(this.data_simple1, this.data_simple1)
        this.assertEqual(len(diff.new     ), 0, "No edits: False positive")
        this.assertEqual(len(diff.removed ), 0, "No edits: False positive")
        this.assertEqual(len(diff.outdated), 0, "No edits: False positive")
        this.assertEqual(len(diff.upToDate), 1, "No edits: False negative")

    def test_upstream_edited(this):
        diff = source_discovery.ProjectDiff(this.data_includes1, this.data_includes2)
        this.assertEqual(len(diff.new     ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.removed ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.outdated), 2, "Upstream edits: False negative")
        this.assertEqual(len(diff.upToDate), 0, "Upstream edits: False positive")
        
    def test_upstream_added(this):
        diff = source_discovery.ProjectDiff(this.data_includes1, this.data_includes3)
        this.assertEqual(len(diff.new     ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.removed ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.outdated), 1, "Upstream edits: False negative")
        this.assertEqual(len(diff.upToDate), 1, "Upstream edits: False positive")
    
    def test_upstream_added(this):
        diff = source_discovery.ProjectDiff(this.data_includes3, this.data_includes1)
        this.assertEqual(len(diff.new     ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.removed ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.outdated), 1, "Upstream edits: False negative")
        this.assertEqual(len(diff.upToDate), 1, "Upstream edits: False positive")


if __name__ == '__main__':
    unittest.main()
