#!/usr/bin/env python3

import os

print("Loading ScriptHost source.")
file = open('../apps/abyssengine/src/engine/scripthost.h', 'r')
headerLines = file.readlines()
file.close()

file = open('../apps/abyssengine/src/engine/scripthost.cpp', 'r')
sourceLines = file.readlines()
file.close()

typesToDefine = []
headerItems = []

output = "---@meta\n"
output += "-- version: 0.1\n\n"
output += "---@class abyss\n"
output += "abyss = {}\n\n"
output += "\n\n-- THIS FILE WAS AUTO-GENERATED BY generate.py\n\n"

def fixVariableTypeName(value):
    return value.replace("std::string_view", "string") \
        .replace("std::string", "string")\
        .replace("std::u16string", "string")\
        .replace("sol::safe_function", "function") \
        .replace("std::unique_ptr<", "") \
        .replace(">", "") \
        .replace("LibAbyss::", "") \
        .replace("int", "number")\
        .replace("bool", "boolean")\
        .replace("float", "number")\
        .replace("double", "number")

def processGlobalFunction(curLine, curLineNum):
    global output
    global sourceLines

    parts = curLine.split("\"")
    luaFunctionName = curLine.split("\"")[1]
    cFunctionName = curLine.split("::")[1].split(",")[0]

    # Find the sourceLines item that contains cFunctionName
    cDefLine = ""
    headerLineIdx = -1
    for x in range(0, len(headerLines)):
        if (" " + cFunctionName + "(") in headerLines[x]:
            cDefLine = headerLines[x]
            headerLineIdx = x
            break

    if cDefLine == "":
        return

    params = cDefLine.split("(")[1].split(")")[0].split(",")
    paramsTypes = cDefLine.split("(")[1].split(")")[0].split(",")
    if len(params) == 1 and params[0] == "":
        params = []

    if len(params) > 0:
        for x in range(0, len(params)):
            paramParts = params[x].strip().split(" ")
            params[x] = paramParts[len(paramParts) - 1]
            paramsTypes[x] = fixVariableTypeName(paramParts[len(paramParts) - 2])
            params[x] = params[x].replace("*", "").replace("&", "")

    functionComment = "---No comment defined for this function in scripthost.h:" + str(headerLineIdx+1) + "\n"
    functionLine = "function abyss." + luaFunctionName + "(" + ", ".join(params) + ") end"

    topLineIdx = -1
    for idx in range(headerLineIdx - 1, 0, -1):
        if "///" not in headerLines[idx]:
            break
        if "\\brief " in headerLines[idx]:
            topLineIdx = idx
            break

    if topLineIdx >= 0:
        functionComment = ""
        for idx in range(topLineIdx, headerLineIdx):
            infoLine = headerLines[idx]
            if "\\brief" in infoLine:
                functionComment += "---" + infoLine.replace("/// \\brief ", "").strip() + "\n"
            elif "\\param" in infoLine:
                paramIndex = -1
                try:
                    paramIndex = params.index(infoLine.split("\\param ")[1].split(" ")[0])
                except:
                    raise Exception("Invalid parameter name '" + infoLine.split("\\param ")[1].split(" ")[0] + "' in scripthost.h:" + str(idx+1))

                if paramIndex > -1:
                    paramName = params[paramIndex]
                    paramType = paramsTypes[paramIndex]
                    # Get all of the text in infoLine after the second space
                    paramComment = infoLine.split("\param " + paramName + " ")[1]


                    functionComment += "---@param " + paramName + " " + paramType + " @ " + paramComment.strip() + "\n"
            elif "\\return" in infoLine:
                returnType = headerLines[headerLineIdx].strip().split(" ")[0]
                returnType = fixVariableTypeName(returnType)

                if returnType[0].isupper() and returnType not in typesToDefine:
                    typesToDefine.append(returnType)

                functionComment += "---@return " + returnType + " @ " + infoLine.replace("/// \\return ", "").strip() + "\n"
    output += functionComment + functionLine + "\n\n"


def processTopLevelFile():
    for i in range(len(sourceLines)):
        curLine = sourceLines[i].strip()

        if curLine.startswith("module.set_function"):
            processGlobalFunction(curLine, i + 1)


# Get a list of all the files included in the header
def loadHeaderItems():
    global headerLines
    global headerItems

    for line in headerLines:
        if line.strip().startswith("#include \""):
            headerItems.append(os.path.abspath(__file__) + os.path.sep + os.path.sep.join(["..", "..", "apps", "abyssengine", "src", "engine"]) + os.path.sep + line.strip().split("\"")[1])
        elif line.strip().startswith("#include <"):
            p = line.strip().split("<")[1].split(">")[0]
            if not p.startswith("libabyss"):
                continue
            headerItems.append(os.path.abspath(__file__) + os.path.sep + ".." + os.path.sep + ".." + os.path.sep + p)

    for i in range(len(headerItems)):
        headerItems[i] = os.path.normpath(headerItems[i])



# Process the top level file
print("Processing top level Abyss functions...")
processTopLevelFile()

print("Processing header files...")
loadHeaderItems()

# Process the class types from typesToDefine
for typeName in typesToDefine:
    headerFile = ""
    for headerItem in headerItems:
        if typeName.lower() + ".h" not in headerItem:
            continue
        headerFile = headerItem

    if headerFile == "":
        raise Exception("Could not find header file for class type " + typeName)

    outputFile = "./library/" + typeName + ".lua"
    classOutput = "---@meta\n\n"
    classOutput += "---@class " + typeName + "\n"
    classOutput += typeName + " = {\n"

    # Find the class item in the source file
    parentIndex = -1
    for i in range(len(sourceLines)):
        if (typeName + ">") in sourceLines[i].strip():
            parentIndex = i
            break

    if parentIndex == -1:
        raise Exception("Could not find class item for class type " + typeName)
    typeNameVar = sourceLines[parentIndex].strip().split(" ")[1]
    print("Processing class type " + typeName + " from '" + headerFile + "' into '" + outputFile + "'...")

    for i in range(len(sourceLines)):
        if not sourceLines[i].strip().startswith(typeNameVar + "["):
            continue

        if "sol::property(" not in sourceLines[i]:
            continue

        functionName = sourceLines[i].strip().split("::")[2].split(",")[0].strip()

        file = open(headerFile, 'r')
        classHeaderLines = file.readlines()
        file.close()

        classFuncIdx = -1
        for hi in range(len(classHeaderLines)):
            if " " + functionName + "(" not in classHeaderLines[hi].strip():
                continue
            classFuncIdx = hi
            break

        if classFuncIdx == -1:
            raise Exception("Could not find function " + functionName + " in header file " + headerFile)

        classFuncReturnType = classHeaderLines[classFuncIdx].strip().split(" ")[0]
        if classFuncReturnType.startswith("[["):
            classFuncReturnType = classHeaderLines[classFuncIdx].strip().split(" ")[1]
        valueName = sourceLines[i].strip().split("\"")[1]

        classOutput += "\n"
        if "\brief " not in  classHeaderLines[classFuncIdx-1].strip():
            classOutput += "    ---Description missing for " + functionName + " in " + os.path.basename(headerFile) + ":" + str(classFuncIdx+1) + "\n"
        else:
            classOutput += "    ---" + classHeaderLines[classFuncIdx-1].strip().split("\\brief")[1].strip() + "\n"

        classOutput += "    ---@type " + fixVariableTypeName(classFuncReturnType) + "\n"
        classOutput += "    " + valueName + ",\n"

    classOutput += "}\n\n"

    for i in range(len(sourceLines)):
        if not sourceLines[i].strip().startswith(typeNameVar + "["):
            continue

        if ("] = &" + typeName + "::") not in sourceLines[i]:
            continue



        scriptFunctionName = sourceLines[i].strip().split("\"")[1]
        functionName = sourceLines[i].strip().split("::")[1].split(";")[0].strip()


        # TODO: Function parameters and node parameters for node stuff

        classOutput += "function " + typeName + ":" + scriptFunctionName + "() end\n"

    classFile = open("./library/" + typeName + ".lua", 'w')
    classFile.write(classOutput)
    classFile.close()

print("Done.")

textFile = open("./library/abyss.lua", "w")
textFile.write(output)
textFile.close()
