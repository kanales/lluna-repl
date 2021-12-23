local repl = repl

local reserved = {
	["and"] = true,
	["break"] = true,
	["do"] = true,
	["else"] = true,
	["elseif"] = true,
	["end"] = true,
	["false"] = true,
	["for"] = true,
	["function"] = true,
	["if"] = true,
	["in"] = true,
	["local"] = true,
	["nil"] = true,
	["not"] = true,
	["or"] = true,
	["repeat"] = true,
	["return"] = true,
	["then"] = true,
	["true"] = true,
	["until"] = true,
	["while"] = true,
}

local function strip_ansi(s)
	return s:gsub("\x1b[[]%d*m", "")
end

local function handle_word(w)
	if reserved[w] then
		return "\x1b[35m" .. w .. "\x1b[39m"
	end
	if _G[w] ~= nil then
		return "\x1b[32m" .. w .. "\x1b[39m"
	end
	return w
end

local function handle_parens(w)
	return "\x1b[34m" .. w .. "\x1b[39m"
end

local function handle_string(w)
	return "\x1b[33m" .. strip_ansi(w) .. "\x1b[39m"
end

function repl.before_print(s)
	return s
		:gsub("(%a+)", handle_word)
		:gsub("[()]", handle_parens)
		:gsub('(".*")', handle_string)
end

function repl.execute(s)
	if not s:match "^return" and not s:match "^function" then
		s = "return " .. s
	end
	local f, err = loadstring(s)
	if not f then
		return err
	end
	local res = f()
	if res == nil then
		return ""
	end
	return tostring(res)
end

function repl.complete(w)
	return w
end
