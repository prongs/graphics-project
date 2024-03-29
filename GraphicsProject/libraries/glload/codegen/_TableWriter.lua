--This file exports a funciton, WriteTable, that writes a given table out to a given file handle.

require "_util";

local writeKey = {};

function writeKey.string(hFile, value, iRecursion)
	WriteFormatted(hFile, "[\"%s\"]", value);
end

function writeKey.number(hFile, value, iRecursion)
	WriteFormatted(hFile, "[%i]", value);
end

local writeValue = {};

function writeValue.string(hFile, value, iRecursion)
	WriteFormatted(hFile, "[==[%s]==]", value);
end

function writeValue.number(hFile, value, iRecursion)
	WriteFormatted(hFile, "%i", value);
end

function writeValue.boolean(hFile, value, iRecursion)
	if(value) then hFile:write("true"); else hFile:write("false"); end;
end

function writeValue.table(hFile, value, iRecursion)
	WriteTable(hFile, value, iRecursion)
end

local function WriteTabs(hFile, iRecursion)
	for iCount = 1, iRecursion, 1 do
		hFile:write("\t");
	end
end

function WriteTable(hFile, outTable, iRecursion)
	if(iRecursion == nil) then iRecursion = 1; end
	
	hFile:write("{\n");
	
	local bHasArray = false;
	local arraySize = 0;
	
	if(#outTable > 0) then bHasArray = true; arraySize = #outTable; end;
	
	for key, value in pairs(outTable) do
		if(writeKey[type(key)] == nil) then print("Malformed table key."); return; end
		if(writeValue[type(value)] == nil) then
			print( string.format("Bad value in table: key: '%s' value type '%s'.", key, type(value)));
			return;
		end
		
		--If the key is not an array index, process it.
		if((not bHasArray) or
				(type(key) ~= "number") or
				not((1 <= key) and (key <= arraySize))) then
			WriteTabs(hFile, iRecursion);
			writeKey[type(key)](hFile, key, iRecursion + 1);
			hFile:write(" = ");
			writeValue[type(value)](hFile, value, iRecursion + 1);
			
			hFile:write(",\n");
		end
	end
	
	if(bHasArray) then
		for i, value in ipairs(outTable) do
			WriteTabs(hFile, iRecursion);
			writeValue[type(value)](hFile, value, iRecursion + 1);
			hFile:write(",\n");
		end
	end

	WriteTabs(hFile, iRecursion - 1);
	hFile:write("}");
end
