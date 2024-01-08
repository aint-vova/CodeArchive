---------------------------------------------------------------- Utility

local function multiplyString(str, amount)
    local newStr = ""
    for n = 1, amount do
        newStr = newStr .. str
    end
    return newStr
end

local function decorate(value)
    if type(value) == "string" then return [["]] .. value .. [["]]
    else return tostring(value) end
end

local function printTable(tbl, indent)
    if indent == nil then indent = 0 end
    for k, v in pairs(tbl) do
        print(multiplyString("  ", indent) .. "[" .. decorate(k) .. "]" .. ": " .. decorate(v))
        if type(v) == "table" then printTable(v, indent + 1) end
    end
end

local function split(str, sep)
   local result = {}
   local regex = ("([^%s]+)"):format(sep)
   for each in str:gmatch(regex) do
      table.insert(result, each)
   end
   return result
end

local function startsWith(str, substr) return string.sub(str, 1, #substr) == substr end

local function cut(str, substr)
    if type(substr) == "number" then return string.sub(str, 2, #str) end
    if startsWith(str, substr) then
         return string.sub(str, #substr + 1, #str)
    else
        error("str didn't start with substr in cut(str, substr) call! substr = " .. tostring(substr) .. ", str slice = " .. tostring(string.sub(str, 1, #substr)))
        return str
    end
end

local function addToken(tokens, code, token)
    table.insert(tokens, token)
    return cut(code, token)
end

local function addTokenWithoutCut(tokens, token)
    table.insert(tokens, token)
end

local function isAlphabeticCharacter(character)
    return string.match(character, "^[a-zA-Z]+$") ~= nil
end

local function isDigitCharacter(character)
    return string.match(character, "^[0-9]+$") ~= nil
end

local function addStringLiteralToken(tokens, code)
    local token = string.sub(code, 1, string.find(code, "\"", 2))
    table.insert(tokens, token)
    return cut(code, token)
end

local function addNumberLiteralToken(tokens, code)
    
    local pointer = 1
    local hasDot = false
    
    while true do
        local character = string.sub(code, pointer, pointer)
        if isDigitCharacter(character) then goto continue end
        if character == "." then
            if not hasDot then
                hasDot = true
                goto continue
            else
                error("Failed to lex the code - a number has two dots :D")
            end
        end
        
        break

        ::continue::
        pointer = pointer + 1
    end
    
    local token = string.sub(code, 1, pointer - 1)
    table.insert(tokens, token)
    return cut(code, token)
end

---------------------------------------------------------------- Lexer

local function lex(code)
    
    local tokens = {}
    
    local symbolStorage = ""
    local writingSymbol = false
    
    while #code > 0 do
        local firstCharacter = string.sub(code, 1, 1)
        if isAlphabeticCharacter(firstCharacter) or firstCharacter == "_" or (writingSymbol and isDigitCharacter(firstCharacter)) then
            writingSymbol = true
            symbolStorage = symbolStorage .. firstCharacter
            code = cut(code, 1)
            goto continue
        else
            if writingSymbol then
                addTokenWithoutCut(tokens, symbolStorage)
                writingSymbol = false
                symbolStorage = ""
                goto continue
            end
        end
        
        if startsWith(code, " ") then code = cut(code, " ") goto continue end
        if startsWith(code, "\r") then code = cut(code, "\r") goto continue end
        if startsWith(code, "\n") then code = cut(code, "\n") goto continue end
        if startsWith(code, "\t") then code = cut(code, "\t") goto continue end
        if startsWith(code, "\v") then code = cut(code, "\v") goto continue end
        
        if startsWith(code, "(") then code = addToken(tokens, code, "(") goto continue end
        if startsWith(code, ")") then code = addToken(tokens, code, ")") goto continue end
        if startsWith(code, ".") then code = addToken(tokens, code, ".") goto continue end
        if startsWith(code, "=") then code = addToken(tokens, code, "=") goto continue end
        if startsWith(code, "\"") then code = addStringLiteralToken(tokens, code) goto continue end
        if isDigitCharacter(string.sub(code, 1, 1)) then code = addNumberLiteralToken(tokens, code) goto continue end

        error("Unknown lex error at: " .. code)
        break
        
        ::continue::
    end
    
    return tokens
end

---------------------------------------------------------------- Program

local code = [[
print("yeah")
int val =     4 int not_val = 8
float myValue = 35.0
int test=36.01
print(val as IBazeengable)
sus.kek("wow")
]]

local tokens = lex(code)

printTable(tokens)
