package com.hikvision.processors;

import com.google.auto.service.AutoService;
import com.hikvision.annotations.TypeAnnotation;
import com.sun.source.tree.Tree;
import com.sun.source.util.Trees;
import com.sun.tools.javac.code.Flags;
import com.sun.tools.javac.model.JavacElements;
import com.sun.tools.javac.processing.JavacProcessingEnvironment;
import com.sun.tools.javac.tree.JCTree;
import com.sun.tools.javac.tree.TreeMaker;
import com.sun.tools.javac.tree.TreeTranslator;
import com.sun.tools.javac.util.Context;
import com.sun.tools.javac.util.List;
import com.sun.tools.javac.util.Name;
import com.sun.tools.javac.util.Names;

import java.lang.annotation.Annotation;
import java.util.HashMap;
import java.util.LinkedHashSet;
import java.util.Map;
import java.util.Set;

import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.Filer;
import javax.annotation.processing.Messager;
import javax.annotation.processing.ProcessingEnvironment;
import javax.annotation.processing.Processor;
import javax.annotation.processing.RoundEnvironment;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.annotation.processing.SupportedSourceVersion;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.PackageElement;
import javax.lang.model.element.TypeElement;
import javax.tools.Diagnostic;

@AutoService(Processor.class)
public class ApiProcessor extends AbstractProcessor {

    private Filer filer;
    private Trees trees;
    private TreeMaker treeMaker;
    private Name.Table table;
    private Messager messager;
    List<JCTree.JCStatement> jcBlockStatements = null;
    //private JavacElements javacElements;

    @Override
    public synchronized void init(ProcessingEnvironment processingEnv) {
        super.init(processingEnv);
        filer = processingEnv.getFiler();
        messager = processingEnv.getMessager();
        trees = Trees.instance(processingEnv);
        Context context = ((JavacProcessingEnvironment)
                processingEnv).getContext();
        treeMaker = TreeMaker.instance(context);
        //javacElements = JavacElements.instance(context);
        table = Names.instance(context).table;//Name.Table.instance(context);
        messager.printMessage(Diagnostic.Kind.NOTE, "Printing: init");
    }

    @Override
    public boolean process(Set<? extends TypeElement> set, RoundEnvironment roundEnvironment) {
        messager.printMessage(Diagnostic.Kind.NOTE, "Printing: process");
        if (!roundEnvironment.processingOver()) {
            messager.printMessage(Diagnostic.Kind.NOTE, "A");
            try {
                processCheck(roundEnvironment);
            } catch (Exception e) {
                e.printStackTrace();
                //error(e.getMessage());
            }
        } else {
            messager.printMessage(Diagnostic.Kind.NOTE, "B");
            System.out.println("logCode = " + logCode);
            for (Map.Entry<String, JCTree.JCMethodDecl> entry : targetMethod.entrySet()) {
                System.out.println("Key = " + entry.getKey() + ", Value = " + entry.getValue());
                JCTree.JCStatement jCStatement = logCode.get(0);
                //jCStatement.getTree().accept();
                entry.getValue().body.stats = entry.getValue().body.stats.prependList(logCode);
            }
            for (Map.Entry<String, JCTree.JCMethodDecl> entry : targetMethod.entrySet()) {
                System.out.println("Key = " + entry.getKey() + ", Value = " + entry.getValue());
            }
        }
        return false;
    }

    private HashMap<String, List<JCTree.JCStatement>> codes = new HashMap<>();

    public Set<String> getSupportedAnnotationTypes() {
        LinkedHashSet<String> annotations = new LinkedHashSet<>();
        annotations.add(TypeAnnotation.ANNOTATION_CHECK);
        annotations.add(TypeAnnotation.ANNOTATION_APIMANAGER);
        annotations.add(TypeAnnotation.ANNOTATION_LOG);
        return annotations;
    }

    public SourceVersion getSupportedSourceVersion() {
        return SourceVersion.RELEASE_7;
    }

    private void processCheck(RoundEnvironment roundEnvironment) throws ClassNotFoundException {
        for (Element element : roundEnvironment.getElementsAnnotatedWith((Class<? extends Annotation>) Class.forName(TypeAnnotation.ANNOTATION_LOG))) {
            if (element.getKind() == ElementKind.CLASS) {
                //获取日志方法代码
                TypeElement typeElement = (TypeElement) element;
                JCTree tree = (JCTree) trees.getTree(typeElement);
                TreeTranslator testBlock = new logCodeBlock();
                tree.accept(testBlock);
            }
        }
        for (Element element : roundEnvironment.getElementsAnnotatedWith((Class<? extends Annotation>) Class.forName(TypeAnnotation.ANNOTATION_CHECK))) {
            if (element.getKind() == ElementKind.CLASS) {
                TypeElement typeElement = (TypeElement) element;
                //com.hikvision.dmb.system.InfoSystemManager
                messager.printMessage(Diagnostic.Kind.NOTE, "typeElement: " + element.toString());
                PackageElement packageElement = (PackageElement) typeElement.getEnclosingElement();
                //
                String cQualifiedName = typeElement.getQualifiedName().toString();
                //InfoSystemManager
                String simpleName = typeElement.getSimpleName().toString();
                //包名 com.hikvision.dmb.system
                String pQualifiedName = packageElement.getQualifiedName().toString();
//                JCTree tree = (JCTree) trees.getTree(variableElement);
//                TreeTranslator visitor = new commonBlock();
//                tree.accept(visitor);
                processingEnv.getMessager().printMessage(Diagnostic.Kind.NOTE, "annatated class : packageName = " + pQualifiedName
                        + " , className = " + simpleName + " , fqClassName = " + cQualifiedName);
                JCTree tree = (JCTree) trees.getTree(typeElement);
                TreeTranslator visitor = new targetMethodBlock();
                tree.accept(visitor);
            } //else
            //error("ActivityInject only can use  in ElementKind.CLASS");
        }
        //
        for (Element element : roundEnvironment.getElementsAnnotatedWith((Class<? extends Annotation>) Class.forName(TypeAnnotation.ANNOTATION_APIMANAGER))) {
            if (element.getKind() == ElementKind.CLASS) {
                //getSerialNumber()
                //field type
                JCTree tree = (JCTree) trees.getTree(element);
                apiMethodBlock api = new apiMethodBlock();
                tree.accept(api);
            } //else
            //error("ActivityInject only can use  in ElementKind.CLASS");
        }
    }

    private class apiMethodBlock extends TreeTranslator {
        public void visitMethodDef(JCTree.JCMethodDecl jcMethodDecl) {
            super.visitMethodDef(jcMethodDecl);
            //如果方法名叫做getUserName则把它的名字修改成testMethod
            if (jcMethodDecl.getName().toString().equals("setLogo")) {
                messager.printMessage(Diagnostic.Kind.NOTE, "setLogo: setLogo");
//                Set<Modifier> flags = jcMethodDecl.getModifiers().getFlags();
//                flags.remove(Modifier.PUBLIC);
//                flags.add(Modifier.PRIVATE);
                messager.printMessage(Diagnostic.Kind.NOTE, "setLogo: " + jcMethodDecl.getModifiers().flags);
                jcMethodDecl.getModifiers().flags &= ~Flags.PUBLIC;
                jcMethodDecl.getModifiers().flags |= Flags.PRIVATE;
                messager.printMessage(Diagnostic.Kind.NOTE, "setLogo: " + jcMethodDecl.getModifiers().flags);
                JCTree.JCMethodDecl methodDecl = treeMaker.MethodDef(jcMethodDecl.getModifiers(), jcMethodDecl.getName(), jcMethodDecl.restype, jcMethodDecl.getTypeParameters()
                        , jcMethodDecl.getParameters(), jcMethodDecl.getThrows(), jcMethodDecl.getBody(), jcMethodDecl.defaultValue);
                result = methodDecl;
            }
        }
    }
//    private void getAnnotatedClass(Element element) {
    // tipe . can not use chines  so  ....
    // get TypeElement  element is class's --->class  TypeElement typeElement = (TypeElement) element
    //  get TypeElement  element is method's ---> TypeElement typeElement = (TypeElement) element.getEnclosingElement();
//        TypeElement typeElement = (TypeElement) element;
//        String fullName = typeElement.getQualifiedName().toString();
//        AnnotatedClass annotatedClass = mAnnotatedClassMap.get(fullName);
//        if (annotatedClass == null) {
//            annotatedClass = new AnnotatedClass(typeElement, mElementUtils, mMessager);
//            mAnnotatedClassMap.put(fullName, annotatedClass);
//        }
//        JCTree tree = (JCTree) trees.getTree(element);
//        TreeTranslator visitor = new Inliner();
//        tree.accept(visitor);
//    }

    List<JCTree.JCStatement> logCode = null;

    private class logCodeBlock extends TreeTranslator {
        public void visitMethodDef(JCTree.JCMethodDecl jcMethodDecl) {
            super.visitMethodDef(jcMethodDecl);
            //如果方法名叫做getUserName则把它的名字修改成testMethod
            if (jcMethodDecl.getName().toString().equals("print")) {
                JCTree.JCBlock jCBlock = jcMethodDecl.getBody();
                jcBlockStatements = jCBlock.stats;
                logCode = List.from(jcBlockStatements);
            }
        }
    }

    private class targetMethodBlock extends TreeTranslator {
        public void visitMethodDef(JCTree.JCMethodDecl jcMethodDecl) {
            super.visitMethodDef(jcMethodDecl);
//            if (jcMethodDecl.getName().toString().equals("<init>")) {
//                return;
//            }
//            messager.printMessage(Diagnostic.Kind.NOTE
//                    , "Modifiers :" + jcMethodDecl.getModifiers().toString());
            if (!jcMethodDecl.getModifiers().toString().contains("static"))
                targetMethod.put(jcMethodDecl.getName().toString(), jcMethodDecl);
        }
    }

    private HashMap<String, JCTree.JCMethodDecl> targetMethod = new HashMap<>();

//    private class logBlock extends TreeTranslator {
//
//        public void visitMethodDef(JCTree.JCMethodDecl jcMethodDecl) {
//            super.visitMethodDef(jcMethodDecl);
//            if (jcMethodDecl.getName().toString().equals("<init>")) {
//                return;
//            }
//            //如果方法名叫做getUserName则把它的名字修改成testMethod
//            JCTree.JCBlock jCBlock = jcMethodDecl.getBody();
//            List<JCTree.JCStatement> out = List.nil();
//            messager.printMessage(Diagnostic.Kind.NOTE, jcMethodDecl.getName() + " befor " + jCBlock.stats.size());
//            messager.printMessage(Diagnostic.Kind.NOTE, jCBlock.stats.toString());
//            if (jcBlockStatements != null && jcBlockStatements.size() > 0) {
//                messager.printMessage(Diagnostic.Kind.NOTE, "Printing:jcBlockStatemen size " + jcBlockStatements.size());
//                for (JCTree.JCStatement statement : jcBlockStatements) {
//                    if (statement == null) {
//                        messager.printMessage(Diagnostic.Kind.NOTE, "Printing: statement == null");
//                        continue;
//                    }
//                }
//                out = out.appendList(jcBlockStatements);
//            }
//            if (jCBlock.stats != null && jCBlock.stats.size() > 0) {
//                for (JCTree.JCStatement statement : stats) {
//                    if (statement == null) {
//                        continue;
//                    }
//                    //tail.add(statement);
//                }
//                out = out.appendList(jCBlock.stats);
//            }

//            SimpleName nameSystem = new SimpleName("System");
//            SimpleName nameOut = new SimpleName("out");
//            SimpleName namePrintln = new SimpleName("println");
    //QualifiedName nameSystemOut = new QualifiedName("System", "out");
    // CompilationUnit compilationUnit = JavaParser.parse("System.out.print(\"dawdasd\")");
    // add a statement do the method body
//            NameExpr clazz = new NameExpr("System");
//            FieldAccessExpr field = new FieldAccessExpr(clazz, "out");
//            MethodCallExpr call = new MethodCallExpr(field, "println");
    //jCBlock.stats.add();

//            jCBlock.stats = out;
//            messager.printMessage(Diagnostic.Kind.NOTE, "out " + out.size());
//            messager.printMessage(Diagnostic.Kind.NOTE, "after " + jCBlock.stats.size());
//            messager.printMessage(Diagnostic.Kind.NOTE, jCBlock.stats.toString());
//            JCTree.JCMethodDecl methodDecl = treeMaker.MethodDef(jcMethodDecl.getModifiers()
//                    , jcMethodDecl.getName(), jcMethodDecl.restype, jcMethodDecl.getTypeParameters(), jcMethodDecl.getParameters()
//                    , jcMethodDecl.getThrows(), jCBlock, jcMethodDecl.defaultValue);
//            result = methodDecl;
//        }
//    }

//    private class commonBlock extends TreeTranslator {
//        public void visitAssert(JCTree.JCAssert tree) {
//            super.visitAssert(tree);
//        }
//
//        public void visitBlock(JCTree.JCBlock jcBlock) {
//            super.visitBlock(jcBlock);
//            if (jcBlockStatements == null)
//                return;
//            List<JCTree.JCStatement> statements = jcBlock.getStatements();
//            if (statements != null && statements.size() > 0) {
//                for (JCTree.JCStatement statement : jcBlock.getStatements()) {
//                    jcBlockStatements = jcBlockStatements.append(statement);
//                    messager.printMessage(Diagnostic.Kind.NOTE, "Printing: A" + statement.toString());
//                }
//                messager.printMessage(Diagnostic.Kind.NOTE, "Printing: A" + jcBlockStatements.size());
//            }
//        }
//    }

//    private class Inliner extends TreeTranslator {
//        public void visitAssert(JCTree.JCAssert tree) {
//            super.visitAssert(tree);
//            JCTree.JCStatement newNode = makeIfThrowException(tree);
//            result = newNode;
//        }

//        public void visitMethodDef(JCTree.JCMethodDecl jcMethodDecl) {
//            super.visitMethodDef(jcMethodDecl);
    //如果方法名叫做getUserName则把它的名字修改成testMethod
//            if (jcMethodDecl.getName().toString().equals("getUserName")) {
//                JCTree.JCMethodDecl methodDecl = treeMaker.MethodDef(jcMethodDecl.getModifiers(), table.fromString("testMethod"), jcMethodDecl.restype, jcMethodDecl.getTypeParameters(), jcMethodDecl.getParameters(), jcMethodDecl.getThrows(), jcMethodDecl.getBody(), jcMethodDecl.defaultValue);
//                result = methodDecl;
//            }
//        }
//
//        public void visitBlock(JCTree.JCBlock jcBlock) {
//            super.visitBlock(jcBlock);
//            List<JCTree.JCStatement> statements = jcBlock.getStatements();
//            if (statements != null && statements.size() > 0) {
//                List<JCTree.JCStatement> out = List.nil();
//                messager.printMessage(Diagnostic.Kind.NOTE, "Printing: B" + jcBlockStatements.size());
//                if (jcBlockStatements != null && jcBlockStatements.size() > 0) {
//                    for (JCTree.JCStatement statement : jcBlockStatements) {
//                        if (statement == null) {
//                            messager.printMessage(Diagnostic.Kind.NOTE, "Printing: statement == null");
//                            continue;
//                        }
//                        if (out == null) {
//                            messager.printMessage(Diagnostic.Kind.NOTE, "Printing: out == null");
//                        }
//                        out = out.append(statement);
//                        messager.printMessage(Diagnostic.Kind.NOTE, "Printing: " + statement.toString());
//                    }
//                }
//                out = out.appendList(statements);
////                for (JCTree.JCStatement statement : statements) {
////                    out = out.append(statement);
////                    messager.printMessage(Diagnostic.Kind.NOTE, "Printing: " + statement.toString());
////                }
//                jcBlock.stats = out;
//            }
//            final List<JCTree.JCStatement> statements = jcBlock.getStatements();
//            if (statements != null && statements.size() > 0) {
//                List<JCTree.JCStatement> out = List.nil();
//                for (JCTree.JCStatement statement : statements) {
//                    if (statement.toString().contains("Log.")) {
//                        //mMessager.printMessage(Diagnostic.Kind.WARNING, this.getClass().getCanonicalName() + " 自动清除Log: LogClear:" + statement.toString());
//                    } else {
//                        out = out.append(statement);
//                    }
//                }
//                jcBlock.stats = out;
//            }
//        }

//        private JCTree.JCStatement makeIfThrowException(JCTree.JCAssert node) {
//            // make: if (!(condition) throw new AssertionError(detail);
//            List<JCTree.JCExpression> args = node.getDetail() == null
//                    ? List.<JCTree.JCExpression>nil()
//                    : List.of(node.detail);
//            JCTree.JCExpression expr = treeMaker.NewClass(
//                    null,
//                    null,
//                    treeMaker.Ident(table.fromString("AssertionError")),
//                    args,
//                    null);
//            return treeMaker.If(
//                    treeMaker.Unary(JCTree.Tag.NOT, node.cond),
//                    treeMaker.Throw(expr),
//                    null);
//        }
//    }
}
