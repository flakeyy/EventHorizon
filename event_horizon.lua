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
		forumPosts = nil,
		allProjects = nil,
		allPerks = nil,
		allScripts = nil,
		steam = nil,
		user = {},
		teamScripts = {},
		xpHistory = {}

	}
}

local function refreshCache()

	-- refresh our data
	local apiCall = fantasy.session:api("getMember&scripts&fc2t&steam&xp")
	eh.cache.memberInfo = json.decode(apiCall)

	apiCall = fantasy.session:api("getFC2TProjects")
	eh.cache.allProjects = json.decode(apiCall)

	apiCall = fantasy.session:api("listPerks")
	eh.cache.allPerks = json.decode(apiCall)

	apiCall = fantasy.session:api("getAllScripts")
	eh.cache.allScripts = json.decode(apiCall)

	apiCall = fantasy.session:api("getForumPosts&count=10")
	eh.cache.forumPosts = json.decode(apiCall)

	eh.cache.user = {}
	eh.cache.teamScripts = {}
	eh.cache.xpHistory = {}

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
	eh.cache.user["buddy"] = eh.cache.memberInfo.buddy

    local currentEpoch = os.time(os.date("*t"))
    local currentDay = currentEpoch - currentEpoch % 86400
    for i=0,6 do
        local xpTable = {}
        local timestamp = currentDay - (i * 86400)
        local xpAmount = 0
        for _, xpEvent in pairs(eh.cache.memberInfo.xp_history) do
            if(xpEvent.time >= timestamp and xpEvent.time <= timestamp + 86399) then
            	xpAmount = xpAmount + xpEvent.amount
            end
        end

        xpTable["timestamp"] = timestamp
        xpTable["xp"] = xpAmount
        table.insert(eh.cache.xpHistory, xpTable)
    end

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

	-- set active to true if the script is enabled
	if eh.cache.memberInfo.scripts ~= nil then
        for _, activeScripts in pairs(eh.cache.memberInfo.scripts) do
            for _, allScripts in pairs(eh.cache.allScripts) do
                if activeScripts.id == allScripts.id then
                    allScripts["active"] = true
                    goto continue
                end
            end
            ::continue::
        end
	end

	for _, allProjects in pairs(eh.cache.allProjects) do
		allProjects["active"] = false
		allProjects["script"] = nil
	end

	if eh.cache.memberInfo.fc2t ~= nil then
        for _, activeProjects in pairs(eh.cache.memberInfo.fc2t) do
            for _, allProjects in pairs(eh.cache.allProjects) do
                if activeProjects.id == allProjects.id then
                    allProjects["active"] = true
                    goto continue
                end
            end
            ::continue::
        end
	end

	for _, allPerks in pairs(eh.cache.allPerks) do
	    allPerks["owned"] = false
	end

	if eh.cache.memberInfo.perks ~= nil then
	    for _, ownedPerks in pairs(eh.cache.memberInfo.perks) do
	        for _, allPerks in pairs(eh.cache.allPerks) do
	            if ownedPerks.id == allPerks.id then
	                allPerks["owned"] = true
	                goto continue
				end
	        end
	        ::continue::
	    end
    end

	for _, post in pairs(eh.cache.forumPosts) do
		post["discussion_state"] = nil
		post["node_id"] = nil
		post["post_date"] = nil
		post["post_id"] = nil
		post["thread_id"] = nil
		post["message"] = nil
	end

	fantasy.log("cache refreshed")
end

local function getMemberAsBuddy(username)

	local apiUrl = "getMemberAsBuddy&name="..username.."&no_beauty"
	local apiCall = fantasy.session:api(apiUrl)
	apiCall = json.decode(apiCall)

	local member = {}

	if apiCall.message ~= nil then
		member["message"] = apiCall.message
		goto continue
	end

	member["username"] = apiCall.username
	member["register_date"] = apiCall.register_date
	member["last_activity"] = apiCall.last_activity
	member["session_expiry"] = apiCall.session["expire"]
	member["posts"] = apiCall.posts
	member["score"] = apiCall.score
	member["protection"] = apiCall.protection

	for _, script in pairs(apiCall["scripts"]) do
		-- remove all script information except name & id, that's all we want
		script["script"] = nil
		script["update_notes"] = nil
		script["elapsed"] = nil
		script["author"] = nil
		script["last_update"] = nil
		script["last_bonus"] = nil
		script["software"] = nil
		script["library"] = nil
		script["forums"] = nil
		script["core"] = nil
		script["team"] = nil
	end

	member["scripts_amount"] = #apiCall["scripts"]
	member["scripts"] = apiCall["scripts"]
	member["message"] = "success"

	::continue::
	return json.encode(member)
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
	elseif identifier == "eh_user" then return json.encode(eh.cache.user)
	elseif identifier == "eh_get_xp_history" then return json.encode(eh.cache.xpHistory[data.value + 1])

	-- perk stuff
	elseif identifier == "eh_perks_amount" then return #eh.cache.allPerks
    elseif identifier == "eh_get_perk_json" then return json.encode(eh.cache.allPerks[data.value + 1])

	-- script stuff
	elseif identifier == "eh_scripts_amount" then return #eh.cache.allScripts
	elseif identifier == "eh_get_script_json" then return json.encode(eh.cache.allScripts[data.value + 1])

	-- fc2t stuff
	elseif identifier == "eh_fc2t_amount" then return #eh.cache.allProjects
	elseif identifier == "eh_get_fc2t_json" then return json.encode(eh.cache.allProjects[data.value + 1])

	-- post stuff
	elseif identifier == "eh_posts_amount" then return #eh.cache.forumPosts
	elseif identifier == "eh_get_post_json" then return json.encode(eh.cache.forumPosts[data.value + 1])

	-- buddy stuff
	elseif identifier == "eh_get_member_as_buddy" then return getMemberAsBuddy(data.value)

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