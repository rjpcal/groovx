# $Id$

import os, re, sys, operator

def sort(seq):
    seq.sort()
    return seq


# DirectIncludeMap class -- maintains a cached mapping between source
# files and lists of files that they directly include

class DirectIncludeMap:
    includeRegex = re.compile('^#\s*include\s*"(.*)"$', re.MULTILINE)

    def loadDirectIncludes(self, file):
        theRegex = self.includeRegex

        f = open(file, 'r')
        text = f.read()
        f.close()

        matches = theRegex.findall(text)

        deps = map(operator.add, self.itsPathCopies[:len(matches)], matches)

        includes = filter(os.path.isfile, deps)

        if not len(includes) == len(deps):
            print >>sys.stderr, 'missing dependencies in file: ', file

        self.itsIncludes[file] = includes

    def __init__(self, path):
        self.itsPath = path + '/'
        self.itsPathCopies = (self.itsPath,)*200
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
        self.itsFullIncludes[target][rootdep] = 1

        for dep in self.itsDirectIncludes.get(rootdep):
            self.addRecursiveDeps(target, dep)

    def visitDir(self, arg, dirname, contents):
        if os.path.basename(dirname)[0].isupper():
            return

        for file in contents:
            if not file.endswith('.cc'):
                continue

            fullname = os.path.join(dirname,file)

            assert not self.itsFullIncludes.has_key(fullname)
            self.itsFullIncludes[fullname] = {}

            self.addRecursiveDeps(fullname, fullname)

    objDir = 'obj/' + os.getenv('ARCH') + '/'

    def printTarget(self, file, stream):
        parts = file.split('/')

        assert parts[0] == self.itsPath
        stem = '.'.join(parts[1:])

        (stem, ext) = os.path.splitext(stem)
        assert ext == '.cc'

        ostem = self.objDir + stem

        stream.write('\n\n%s.do %s.o:' % (ostem, ostem))

        for dep in sort(self.itsFullIncludes[file].keys()):
            stream.write(' \\\n\t%s' % dep)

    # public interface

    def __init__(self, path):
        self.itsPath = path
        self.itsFullIncludes = {}
        self.itsDirectIncludes = DirectIncludeMap(path)

    def buildDepTree(self):
        os.path.walk(self.itsPath, self.visitDir, None)

    def printMakeDeps(self, stream):
        header = "# Do not edit this file! It is automatically generated. Changes will be lost."

        stream.write(header)

        for file in sort(self.itsFullIncludes.keys()):
            self.printTarget(file, stream)

        stream.write('\n')
