--[[
	@title
        Event Horizon

    @author
        flakey

    @description
        An all in one menu for Universe4 made with ImGui and FC2T
--]]

local json = require("json")
local modules = require("modules")

local eh = {

	cache = {
		
		last_refresh = os.time(),
	
		memberInfo = nil,
		user = {},
		allProjects = nil,
		allPerks = nil,
		allScripts = nil,
		steam = nil,
		teamScripts = {},

	}
}

local function refreshCache()
	
	-- refresh our data
	local apiCall = fantasy.session:api("getMember&scripts&fc2t&steam")
	eh.cache.memberInfo = json.decode(apiCall)
	
	apiCall = fantasy.session:api("getFC2TProjects")
	eh.cache.allProjects = json.decode(apiCall)
	
	apiCall = fantasy.session:api("listPerks")
	eh.cache.allPerks = json.decode(apiCall)
	
	apiCall = fantasy.session:api("getAllScripts")
	eh.cache.allScripts = json.decode(apiCall)
	
	eh.cache.user = {}
	eh.cache.teamScripts = {}
	
	-- get the user information we need for the "user" call
	eh.cache.user["astrology"] = eh.cache.memberInfo.sign
	eh.cache.user["xp"] = eh.cache.memberInfo.xp
	eh.cache.user["perk_points"] = eh.cache.memberInfo.perk_points
	eh.cache.user["posts"] = eh.cache.memberInfo.posts
	eh.cache.user["score"] = eh.cache.memberInfo.score
	eh.cache.user["unread_conversations"] = eh.cache.memberInfo.unread_conversations
	eh.cache.user["unread_alerts"] = eh.cache.memberInfo.unread_alerts
	eh.cache.user["session_started"] = eh.cache.memberInfo.session["started"]
	eh.cache.user["session_expiry"] = eh.cache.memberInfo.session["expire"]
	
	for _, script in pairs(eh.cache.allScripts) do
		for _, teamMember in pairs(script.team) do
			if(teamMember == fantasy.session.username) then
				table.insert(eh.cache.teamScripts, script)
				goto continue
			end
		end
		::continue::
	end
	
	-- remove script code from scripts and add an "active" key
	for _, allScripts in pairs(eh.cache.allScripts) do
		allScripts["active"] = false
		allScripts["script"] = nil
	end
	
	if eh.cache.memberInfo.scripts == nil then goto skipScripts end
	
	-- set the active key to true if the script is enabled
	for _, activeScripts in pairs(eh.cache.memberInfo.scripts) do
		for _, allScripts in pairs(eh.cache.allScripts) do
			if activeScripts.id == allScripts.id then
				allScripts["active"] = true
				goto continue
			end
		end
		::continue::
	end
	
	::skipScripts::
	
	-- same as we did for scripts, just for fc2t projects
	for _, allProjects in pairs(eh.cache.allProjects) do
		allProjects["active"] = false
		allProjects["script"] = nil
	end
	
	if eh.cache.memberInfo.fc2t == nil then goto skipProjects end
	
	for _, activeProjects in pairs(eh.cache.memberInfo.fc2t) do
		for _, allProjects in pairs(eh.cache.allProjects) do
			if activeProjects.id == allProjects.id then
				allProjects["active"] = true
				goto continue
			end
		end
		::continue::
	end
	
	::skipProjects::
	
	fantasy.log("cache refreshed")
end

function eh.on_loaded()
	
	refreshCache()
	
end

function eh.on_worker()
	
	-- auto-refresh every 2 minutes
	if os.time() - 120 < eh.cache.last_refresh then return end
	eh.cache.last_refresh = os.time()
	
	--refreshCache()

end

function eh.on_team_call(identifier, data)
	
	-- no switch statement type shit
	if identifier == "eh_ready" then return true
	elseif identifier == "eh_refresh" then modules.scripts:reset(true)
	elseif identifier == "eh_refresh_cache" then refreshCache()
	elseif identifier == "eh_user" then   return json.encode(eh.cache.user)
	
	-- perk stuff
	elseif identifier == "eh_perks_amount" then return #eh.cache.allPerks
	elseif identifier == "eh_owned_perks_amount" then if(eh.cache.memberInfo.perks) then return #eh.cache.memberInfo.perks end return 0
	elseif identifier == "eh_get_owned_perk_id" then return eh.cache.memberInfo.perks[data.value + 1].id
	elseif identifier == "eh_get_perk_json" then return json.encode(eh.cache.allPerks[data.value + 1])
	
	-- script stuff
	elseif identifier == "eh_scripts_amount" then return #eh.cache.allScripts
	elseif identifier == "eh_get_script_json" then return json.encode(eh.cache.allScripts[data.value + 1])
	
	-- fc2t stuff
	elseif identifier == "eh_fc2t_amount" then return #eh.cache.allProjects
	elseif identifier == "eh_get_fc2t_json" then return json.encode(eh.cache.allProjects[data.value + 1])
	
	-- -- team stuff
	-- elseif identifier == "eh_team_scripts_amount" then return #eh.cache.teamScripts
	-- elseif identifier == "eh_get_team_script_name" then return eh.cache.teamScripts[data.value + 1].name
	-- elseif identifier == "eh_get_team_script_id" then return eh.cache.teamScripts[data.value + 1].id
	-- elseif identifier == "eh_get_team_script_source" then return eh.cache.teamScripts[data.value + 1].script

	-- -- steam stuff
	-- elseif identifier == "eh_steam_amount" then fantasy.log(#eh.cache.steam) return #eh.cache.memberInfo.steam
	-- elseif identifier == "eh_get_steam_name" then return eh.cache.memberInfo.steam[data.value + 1].name
	-- elseif identifier == "eh_get_steam_persona" then return eh.cache.memberInfo.steam[data.value + 1].persona
		
	end
end

return eh