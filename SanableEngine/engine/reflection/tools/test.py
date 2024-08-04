import typing
import unittest
import os

import source_discovery

class TestDiffs(unittest.TestCase):
    
    @classmethod
    def setUpClass(this):
        this.selfpath = os.path.dirname(os.path.realpath(__file__))

        emptyDir = os.path.join( this.selfpath, "test_data", "diffs", "empty")
        if not os.path.exists(emptyDir): os.mkdir(emptyDir)
            
        os.chdir(emptyDir                                                        ) ; this.data_empty     = source_discovery.Project(".", [])
        os.chdir(os.path.join( this.selfpath, "test_data", "diffs", "simple1"   )) ; this.data_simple1   = source_discovery.Project(".", [])
        os.chdir(os.path.join( this.selfpath, "test_data", "diffs", "simple2"   )) ; this.data_simple2   = source_discovery.Project(".", [])
        os.chdir(os.path.join( this.selfpath, "test_data", "diffs", "includes1" )) ; this.data_includes1 = source_discovery.Project(".", [])
        os.chdir(os.path.join( this.selfpath, "test_data", "diffs", "includes2" )) ; this.data_includes2 = source_discovery.Project(".", [])
        os.chdir(os.path.join( this.selfpath, "test_data", "diffs", "includes3" )) ; this.data_includes3 = source_discovery.Project(".", [])
        os.chdir(this.selfpath)
        assert this.data_simple1.files[0].contents != this.data_simple2.files[0].contents

    def test_fresh(this):
        diff = source_discovery.ProjectDiff(None, this.data_simple2)
        this.assertEqual(len(diff.new     ), 1, "Fresh scan: False negative")
        this.assertEqual(len(diff.removed ), 0, "Fresh scan: False positive")
        this.assertEqual(len(diff.outdated), 0, "Fresh scan: False positive")
        this.assertEqual(len(diff.upToDate), 0, "Fresh scan: False positive")

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

    def test_upstream_same(this):
        diff = source_discovery.ProjectDiff(this.data_includes1, this.data_includes1)
        this.assertEqual(len(diff.new     ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.removed ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.outdated), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.upToDate), 2, "Upstream edits: False negative")

    def test_upstream_edited(this):
        diff = source_discovery.ProjectDiff(this.data_includes1, this.data_includes2)
        this.assertEqual(len(diff.new     ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.removed ), 0, "Upstream edits: False positive")
        this.assertEqual(len(diff.outdated), 2, "Upstream edits: False negative")
        this.assertEqual(len(diff.upToDate), 0, "Upstream edits: False positive")
        
    def test_upstream_added(this):
        diff = source_discovery.ProjectDiff(this.data_includes3, this.data_includes1)
        this.assertEqual(len(diff.new     ), 0, "Upstream added: False positive")
        this.assertEqual(len(diff.removed ), 0, "Upstream added: False positive")
        this.assertEqual(len(diff.outdated), 1, "Upstream added: False negative")
        this.assertEqual(len(diff.upToDate), 1, "Upstream added: False positive")
    
    def test_upstream_removed(this):
        diff = source_discovery.ProjectDiff(this.data_includes1, this.data_includes3)
        this.assertEqual(len(diff.new     ), 0, "Upstream added: False positive")
        this.assertEqual(len(diff.removed ), 0, "Upstream added: False positive")
        this.assertEqual(len(diff.outdated), 1, "Upstream added: False negative")
        this.assertEqual(len(diff.upToDate), 1, "Upstream added: False positive")


import argparse, cx_ast_tooling, cx_ast, abc

class TestParser:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def invoke_parser(target:str, includes:list[str]) -> cx_ast.Module:
        assert False, "Implement this!"
        
    @classmethod    
    def setUpClass(this):
        this.module = this.invoke_parser("test_data/parser", [])
        
    def assertExpectSymbol(this, name:str, _ty:cx_ast.ASTNode) -> cx_ast.ASTNode|None:
        sym = this.module.find(name)
        if _ty == None:
            this.assertIsNone(sym, msg=f"Symbol {name} shouldn't exist")
            return None
        elif sym == None:
            this.assertIsNotNone(sym, msg=f"Symbol {name} doesn't exist'")
            return None
        else:
            this.assertIsInstance(sym, _ty, msg=f"Symbol {name}: expected {_ty}, but was {type(sym)}")
            return sym

    def assertExpectAnnotation(this, sym:cx_ast.ASTNode, selector:typing.Callable[[cx_ast.Annotation], bool], count:int):
        matches = []
        for i in sym.children:
            if isinstance(i, cx_ast.Annotation) and selector(i):
                matches.append(i)
        this.assertTrue(len(matches) == count)
        return matches

    def test_basic_symbols_exist(this):
        this.assertExpectSymbol("::globalFunc(int, char, const void*)", cx_ast.GlobalFuncInfo)
        
        this.assertExpectSymbol("::MyClass", cx_ast.TypeInfo)
        this.assertExpectSymbol("::MyClass::MyClass()", cx_ast.ConstructorInfo)
        this.assertExpectSymbol("::MyClass::~MyClass()", cx_ast.DestructorInfo)
        this.assertExpectSymbol("::MyClass::MyClass(::MyClass const&)", cx_ast.ConstructorInfo)
        this.assertExpectSymbol("::MyClass::foo", cx_ast.FieldInfo)
        this.assertExpectSymbol("::MyClass::bar", cx_ast.FieldInfo)
        this.assertExpectSymbol("::MyClass::myClassFunc(int)", cx_ast.MemFuncInfo)
        this.assertExpectSymbol("::MyClass::myConstClassFunc(int)", cx_ast.MemFuncInfo) # TODO test disambiguation with this-const overloading
        this.assertExpectSymbol("::MyClass::myStaticClassFunc(int, ::MyClass*)", cx_ast.StaticFuncInfo)
        
        mySubclass = this.assertExpectSymbol("::MySubclass", cx_ast.TypeInfo)
        parents = [i for i in mySubclass.children if isinstance(i, cx_ast.ParentInfo)]
        this.assertTrue(len(parents) == 1)
        this.assertTrue(parents[0].parentTypeName == "::MyClass")
        
    def test_implicit_symbols_exist(this):
        this.assertExpectSymbol("::NonDefaulted", cx_ast.TypeInfo)
        this.assertExpectSymbol("::NonDefaulted::NonDefaulted()", None) # Explicit ctor defined, removes implicit default ctor
        this.assertExpectSymbol("::NonDefaulted::NonDefaulted(int)", cx_ast.ConstructorInfo)
        this.assertExpectSymbol("::NonDefaulted::~NonDefaulted()", cx_ast.DestructorInfo) # Implicit dtor

    def test_virtual_method_detection(this):
        # Not virtual
        func:cx_ast.MemFuncInfo = this.assertExpectSymbol("::MyClass::myClassFunc(int)", cx_ast.MemFuncInfo)
        this.assertFalse(func.isVirtual)
        func:cx_ast.MemFuncInfo = this.assertExpectSymbol("::MyClass::myConstClassFunc(int)", cx_ast.MemFuncInfo)
        this.assertFalse(func.isVirtual)
        
        # Explicitly virtual
        func:cx_ast.MemFuncInfo = this.assertExpectSymbol("::MyClass::myVirtualFunc(int)", cx_ast.MemFuncInfo)
        this.assertTrue(func.isVirtual)
        func:cx_ast.MemFuncInfo = this.assertExpectSymbol("::MyClass::myPureVirtualFunc(int)", cx_ast.MemFuncInfo)
        this.assertTrue(func.isVirtual)
        
        # Implicitly virtual
        func:cx_ast.MemFuncInfo = this.assertExpectSymbol("::MySubclass::myVirtualFunc(int)", cx_ast.MemFuncInfo)
        this.assertTrue(func.isVirtual)
        func:cx_ast.MemFuncInfo = this.assertExpectSymbol("::MySubclass::myPureVirtualFunc(int)", cx_ast.MemFuncInfo)
        this.assertTrue(func.isVirtual)
        
    def test_visibility_detection(this):
        # ClassVisibilityTester
        sym:cx_ast.Member = this.assertExpectSymbol("::ClassVisibilityTester::myDefault", cx_ast.Member)
        this.assertTrue(sym.visibility == cx_ast.Member.Visibility.Private)
        sym:cx_ast.Member = this.assertExpectSymbol("::ClassVisibilityTester::myPrivate", cx_ast.Member)
        this.assertTrue(sym.visibility == cx_ast.Member.Visibility.Private)
        sym:cx_ast.Member = this.assertExpectSymbol("::ClassVisibilityTester::myProtected", cx_ast.Member)
        this.assertTrue(sym.visibility == cx_ast.Member.Visibility.Protected)
        sym:cx_ast.Member = this.assertExpectSymbol("::ClassVisibilityTester::myPublic", cx_ast.Member)
        this.assertTrue(sym.visibility == cx_ast.Member.Visibility.Public)
        
        # StructVisibilityTester
        sym:cx_ast.Member = this.assertExpectSymbol("::StructVisibilityTester::myDefault", cx_ast.Member)
        this.assertTrue(sym.visibility == cx_ast.Member.Visibility.Public)
        sym:cx_ast.Member = this.assertExpectSymbol("::StructVisibilityTester::myPrivate", cx_ast.Member)
        this.assertTrue(sym.visibility == cx_ast.Member.Visibility.Private)
        sym:cx_ast.Member = this.assertExpectSymbol("::StructVisibilityTester::myProtected", cx_ast.Member)
        this.assertTrue(sym.visibility == cx_ast.Member.Visibility.Protected)
        sym:cx_ast.Member = this.assertExpectSymbol("::StructVisibilityTester::myPublic", cx_ast.Member)
        this.assertTrue(sym.visibility == cx_ast.Member.Visibility.Public)

    def test_namespaced_exist(this):
        this.assertExpectSymbol("::MyNamespace", cx_ast.Namespace)
        this.assertExpectSymbol("::MyNamespace::globalFuncInNamespace(int, char, const void*)", cx_ast.GlobalFuncInfo)
        this.assertExpectSymbol("::MyNamespace::ClassInNamespace", cx_ast.TypeInfo)
        this.assertExpectSymbol("::MyNamespace::ClassInNamespace::ClassInNamespace()", cx_ast.ConstructorInfo) # Implicit default ctor
        
    def test_annotations_exist(this):
        annotTgt = this.assertExpectSymbol("::annotatedGlobalFunc()", cx_ast.GlobalFuncInfo)
        this.assertExpectAnnotation(annotTgt, lambda a: a.text == "annot_globfunc", 1)
        annotTgt = this.assertExpectSymbol("::AnnotatedClass", cx_ast.TypeInfo)
        this.assertExpectAnnotation(annotTgt, lambda a: a.text == "annot_cls", 1)
        annotTgt = this.assertExpectSymbol("::AnnotatedClass::foo", cx_ast.FieldInfo)
        this.assertExpectAnnotation(annotTgt, lambda a: a.text == "annot_field", 1)
        annotTgt = this.assertExpectSymbol("::AnnotatedClass::annotatedMemFunc()", cx_ast.MemFuncInfo)
        this.assertExpectAnnotation(annotTgt, lambda a: a.text == "annot_memfunc", 1)
        annotTgt = this.assertExpectSymbol("::AnnotatedNamespaceA", cx_ast.Namespace)
        this.assertExpectAnnotation(annotTgt, lambda a: a.text == "annot_ns_a", 1)
        annotTgt = this.assertExpectSymbol("::AnnotatedNamespaceB", cx_ast.Namespace)
        this.assertExpectAnnotation(annotTgt, lambda a: a.text == "annot_ns_b", 2)
        
from cx_ast_clang_reader import ClangParseContext
class TestClangParser(TestParser, unittest.TestCase):
    def invoke_parser(target:str, includes:list[str]):
        stixPath = os.path.dirname(__file__)
        stixPath = stixPath.replace("/", os.path.sep)
        assert os.path.exists(stixPath)
        args = [
            "--verbose",
            "--target" , f"{stixPath}/{target}"                   .replace("/", os.path.sep),
            "--output" , f"{stixPath}/tools/test_tmp/ast.stix-ast".replace("/", os.path.sep), # We'll never read/write this
            "--define" , "PLATFORM_DLL_EXTENSION=\".dll\";CS_ARCH_OURS=CS_ARCH_X86;NOMINMAX"
        ]
        if len(includes) > 0:
            args.append("--include")
            args.append(';'.join([f"{stixPath}/{i}" for i in includes]).replace("/", os.path.sep))
        
        arg_parser = argparse.ArgumentParser()
        ClangParseContext.argparser_add_defaults(arg_parser)
        args = arg_parser.parse_args(args)

        cx_project = source_discovery.Project(
            cx_ast_tooling.reduce_lists(args.targets),
            cx_ast_tooling.reduce_lists(args.includes)
        )
        cx_parser = ClangParseContext(cx_project, args, 0)
        cx_parser.configure()
        cx_parser.ingest()
        return cx_parser.module


if __name__ == '__main__':
    unittest.main()
