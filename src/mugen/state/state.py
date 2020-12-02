class State:
    def __init__(self, name):
        self.name = name
        self.fields = []

    def isPOD(self):
        return False

    def __str__(self):
        return self.name

    def addField(self, field):
        self.fields.append(field)

class Program:
    def __init__(self, includes, namespace, structs):
        self.includes = includes
        self.namespace = namespace
        self.structs = structs

def combineTemplate(name, templates):
    if templates != None:
        # Need a space before > to prevent >> from being lexed as operator>>
        return "%s<%s >" % (name, ', '.join([str(x) for x in templates]))
    return name

def combineModifier(modifier, name):
    if modifier != None:
        return '%s %s' % (modifier, name)
    return name

class Type:
    def __init__(self, namespace, modifier, template, name):
        self.namespace = namespace
        self.modifier = modifier
        self.template = template
        self.name = name

    def isPOD(self):
        return self.name in ['int', 'short', 'char', 'double', 'bool', 'uint32_t']

    def element(self):
        return ', '.join([str(x) for x in self.template])

    def __str__(self):
        me = combineModifier(self.modifier, combineTemplate(self.name, self.template))
        if self.namespace != None:
            return '%s::%s' % (self.namespace, me)
        return me

class Field:
    def __init__(self, type_, name, array = None):
        self.type_ = type_
        self.name = name
        self.array = array

    def isArray(self):
        return self.array != None

    def zero(self):
        if self.type_.name == 'bool':
            return 'false';
        return '0'

def makeType(modifier, name, template, namespace):
    if namespace != None:
        namespace.namespace = Type(None, modifier, template, name)
        return namespace
    return Type(None, modifier, template, name)

    #all = combineModifier(modifier, combineTemplate(name, template))
    #if namespace != None:
    #    return all + "::" + namespace
    #return all
