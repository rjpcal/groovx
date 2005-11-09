# $Id$

import os, re, sys, operator, math

def sort(seq):
    seq.sort()
    return seq

def log2(x):
    return math.log(x) / math.log(2)

def balancedCCD(N):
    return ( (N+1) * (log2(N+1)-1) ) + 1

# DirectIncludeMap class -- maintains a cached mapping between source
# files and lists of files that they directly include

class DirectIncludeMap:
    includeRegex = re.compile('^#\s*include\s*"(.*)"$', re.MULTILINE)

    def isFile(self, path):
        return os.path.isfile(path)

    def isNotFile(self, path):
        return not os.path.isfile(path)

    def resolve(self, rawIncludes, rootPath):
        rootPathCopies = (rootPath,)*200
        fullPaths = map(operator.add, \
                        rootPathCopies[:len(rawIncludes)], rawIncludes)

        resolved = filter(self.isFile, fullPaths)

        if len(resolved) == len(fullPaths):
            return (resolved, [])
        else:
            unresolved = filter(self.isNotFile, fullPaths)
            rootlen = len(rootPathCopies[0])
            for i in range(len(unresolved)):
                unresolved[i] = unresolved[i][rootlen:]
            return (resolved, unresolved)

    def loadDirectIncludes(self, file):
        theRegex = self.includeRegex

        f = open(file, 'r')
        text = f.read()
        f.close()

        unresolved = theRegex.findall(text)
        resolved = []

        for path in self.itsPaths:
            (newResolved, unresolved) = self.resolve(unresolved, path)
            resolved.extend(newResolved)

        # If there are still some failed lookups, try looking in the
        # "ego" directory, i.e. the one containing the source file:
        if len(unresolved) > 0:
            egoPath = os.path.dirname(file) + '/'

            (newResolved, unresolved) = self.resolve(unresolved, egoPath)
            resolved.extend(newResolved)

        # All attempts to resolve have failed, so issue a warning:
        if len(unresolved) > 0:
            sys.stderr.write("missing dependencies in file: %s\n" % file)
            sys.stderr.write("\twith search path: %s\n" % self.itsPaths)
            for dep in unresolved:
                sys.stderr.write("\t'%s'\n" % dep)

        resolved = map(os.path.normpath, resolved)

        self.itsIncludes[file] = resolved

    def __init__(self, paths):
        self.itsPaths = []
        for path in paths:
            path = path + '/'
            self.itsPaths.append(path)

        self.itsIncludes = {}

    def get(self, file):
        if not self.itsIncludes.has_key(file):
            self.loadDirectIncludes(file)

        return self.itsIncludes[file]
        

#
# DepBuilder class
#

class DepBuilder:

    # private interface

    def addRecursiveDeps(self, target, rootdep):
        # use the next line to avoid recursion
        #if not self.itsFullIncludes[target].has_key(rootdep):

        self.itsFullIncludes[target][rootdep] = 1

        for dep in self.itsDirectIncludes.get(rootdep):
            self.addRecursiveDeps(target, dep)

    def visitDir(self, arg, dirname, contents):
        if os.path.basename(dirname)[0].isupper():
            return

        for file in contents:
            if not (file.endswith('.cc')
                    or file.endswith('.C')
                    or file.endswith('.c')):
                continue

            fullname = os.path.join(dirname,file)

            assert not self.itsFullIncludes.has_key(fullname)
            self.itsFullIncludes[fullname] = {}

            self.addRecursiveDeps(fullname, fullname)


    def getObjFilestem(self, ccfilename):
        parts = ccfilename.split('/')

        assert parts[0] == self.itsProjectPath
        stem = '/'.join(parts[1:])

        (stem, ext) = os.path.splitext(stem)
        assert (ext == '.cc' or ext == '.C' or ext == '.c')

        ostem = self.itsObjDir + stem

        return ostem

    def printOneFileDeps(self, file, stream):
        ostem = self.getObjFilestem(file)

        stream.write('\n\n%s.do %s.o:' % (ostem, ostem))

        for dep in sort(self.itsFullIncludes[file].keys()):
            stream.write(' \\\n\t%s' % dep)
        

    # public interface

    def __init__(self, paths, objdir = ('obj/' + os.getenv('ARCH') + '/')):
        self.itsProjectPath = paths[0]
        self.itsFullIncludes = {}
        self.itsDirectIncludes = DirectIncludeMap(paths)
        self.itsCLevels = {}
        self.itsLLevels = {}
        self.itsObjDir = objdir

    def buildDepTree(self):
        os.path.walk(self.itsProjectPath, self.visitDir, None)

    def printMakeDeps(self, stream):
        header = "# Do not edit this file! It is automatically generated. Changes will be lost."

        stream.write(header)

        for file in sort(self.itsFullIncludes.keys()):
            self.printOneFileDeps(file, stream)

        stream.write('\n')

    #
    # Compile-time dependencies
    #

    def getCdepLevel(self, file):
        if not self.itsCLevels.has_key(file):
            
            depends = self.itsDirectIncludes.get(file)

            if len(depends) == 0:
                self.itsCLevels[file] = 0
            else:
                maxlevel = 0
                for dep in depends:
                    deplevel = self.getCdepLevel(dep)
                    if (deplevel + 1) > maxlevel:
                        maxlevel = deplevel + 1
                self.itsCLevels[file] = maxlevel

        return self.itsCLevels[file]

    def printOneCdepLevel(self, file, stream, marks,
                          indentlevel, maxindent, indenter, separator):
        indents = indenter * indentlevel

        if not marks.has_key(file):
            stream.write('%s%s (%d)%s' %
                         (indents, file, self.getCdepLevel(file), separator))
            marks[file] = 1

        if indentlevel < maxindent:
            for dep in sort(self.itsDirectIncludes.get(file)):
                self.printOneCdepLevel(dep, stream, marks,
                                       indentlevel+1, maxindent,
                                       indenter, separator)

    def printCdepLevels(self, stream, maxlevel):
        backmap = {}

        for file in sort(self.itsFullIncludes.keys()):
            level = self.getCdepLevel(file)
            if not backmap.has_key(level):
                backmap[level] = []
            backmap[level].append(file)
            
        marks = {}

        for level in sort(backmap.keys()):
            for file in backmap[level]:
                stream.write('\n\n')
                marks.clear()
                self.printOneCdepLevel(file, stream, marks, 0, maxlevel,
                                       '\t', '\n')

        stream.write('\n')

    #
    # Link-time dependencies
    #

    def getLdepLevel(self, antecedents, file):
        if not self.itsLLevels.has_key(file):

            antecedents.append(file)

            depends = sort(self.itsFullIncludes[file].keys())

            if len(depends) == 0:
                self.itsLLevels[file] = 0
            else:
                maxlevel = 0
                for dep in depends:
                    (stem, ext) = os.path.splitext(dep)
                    dep = stem + '.cc'

                    if (not (dep == file)) and (os.path.isfile(dep)):
                        if (antecedents.count(dep) > 0):
                            sys.stderr.write('CYCLE! %s <--> %s\n' % (dep, file))
                        else:
                            deplevel = self.getLdepLevel(antecedents, dep)
                            if (deplevel + 1) > maxlevel:
                                maxlevel = deplevel + 1

                self.itsLLevels[file] = maxlevel

            antecedents.pop()

        return self.itsLLevels[file]

    def doMarkLdeps(self, file, marks):

        if not marks.has_key(file):
            marks[file] = 1

            for dep in sort(self.itsFullIncludes[file].keys()):
                (stem, ext) = os.path.splitext(dep)
                dep = stem + '.cc'

                if (os.path.isfile(dep)):
                    self.doMarkLdeps(dep, marks)

    def getCD(self, file):
        marks = {}
        self.doMarkLdeps(file, marks)
        return len(marks)

    def printOneLdepLevel(self, file, stream, marks, level, maxlevel,
                          indenter, separator):

        if not marks.has_key(file):
            indents = indenter * level

            if (level <= maxlevel):
                stream.write('%s%s (%d)%s' %
                             (indents, file, self.getLdepLevel([], file),
                              separator))

            marks[file] = 1

            if (level < maxlevel):
                for dep in sort(self.itsFullIncludes[file].keys()):
                    (stem, ext) = os.path.splitext(dep)
                    dep = stem + '.cc'

                    if (os.path.isfile(dep)):
                        self.printOneLdepLevel(dep, stream, marks,
                                               level+1, maxlevel,
                                               indenter, separator)

    def printLdepLevels(self, stream, maxlevel):
        backmap = {}

        N = 0
        CCD = 0

        for file in sort(self.itsFullIncludes.keys()):
            N += 1
            density = self.getCD(file)
            if not backmap.has_key(density):
                backmap[density] = []
            backmap[density].append(file)

        marks = {}

        for density in sort(backmap.keys()):
            for file in backmap[density]:
                stream.write('\n<%d> ' % density)
                marks.clear()
                self.printOneLdepLevel(file, stream, marks, 0, maxlevel,
                                       '\t', '\n')
                CCD += density

        stream.write('\n')

        stream.write('N == %d\n' % N)
        stream.write('CCD == %d\n' % CCD)
        stream.write('ACD == %f\n' % (float(CCD)/N))
        balanced = balancedCCD(N)
        stream.write('CCD(tree) == %f\n' % balanced)
        stream.write('NCCD == %f\n' % (float(CCD)/balanced))