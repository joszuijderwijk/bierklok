ncaps = 4

local function isempty(s)
  return s == nil or s == ''
end

function mysplit (inputstr, sep)
		
		if inputstr == "[]" then
			return {}
		end
		
        if sep == nil then
                sep = "%s"
        end
        local t={}
        for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
				str = str:gsub('%W','')
                table.insert(t, str)
        end
        return t
end

function Initialize()
	bonus = SKIN:GetMeasure('MeasureBonuses')
end

function Update()
	
	list = bonus:GetStringValue()
	bonuses = mysplit(list, ",")
	n = table.getn(bonuses)

	for i = 1, ncaps do
		if i <= n then
			SKIN:Bang('!SetOption', 'Cap'..i, 'ImageName', '#@#\\caps\\' ..bonuses[i]..'.png')
			SKIN:Bang('!SetOption', 'Cap'..i, 'Hidden', 0)
		else
			SKIN:Bang('!SetOption', 'Cap'..i, 'Hidden', 1)
		end
	end
	
	if n > 0 then
		SKIN:Bang('!SetOption', 'NoBonus', 'Hidden', 1)
	else
		SKIN:Bang('!SetOption', 'NoBonus', 'Hidden', 0)
	end

end

