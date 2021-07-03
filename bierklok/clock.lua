
brands = {HertogJan=0, Grolsch=1, Kornuit=2, Brand=3, Warsteiner=4, Bud=5, Heineken=6, Jupiler=7, Alfa=8, Bavaria=9, Amstel=10, Gulpener=11}
supermarkets = {"albertheijn", "coop", "plus", "spar", "boons", "hoogvliet"}

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
	hour = 0
	minute = 0
end

function Update()
	
	list = bonus:GetStringValue()
	bonuses = mysplit(list, ",")
	n = table.getn(bonuses)
	
	if (n == 0) then
		if not isempty(list) then
			NextSupermarket()
		end
	elseif (n == 1) then
		hour = brands[bonuses[1]]
		minute = hour
	elseif (n>=2) then
		hour = brands[bonuses[1]]
		minute = brands[bonuses[2]]
	end

end

function GetHour()
	return hour
end

function GetMinute()
	return minute
end

function NextSupermarket()
	super = SKIN:GetVariable('supermarket', '')
			
	i = 0
	n = 0
	for k,v in ipairs(supermarkets) do
		if v == super then
			i = k
		end
		n = n + 1
	end
	
	i = (i % n) + 1

	SKIN:Bang('!SetVariable', 'supermarket', supermarkets[i])
	SKIN:Bang('!CommandMeasure', 'MeasureKratjes', 'Update')
end