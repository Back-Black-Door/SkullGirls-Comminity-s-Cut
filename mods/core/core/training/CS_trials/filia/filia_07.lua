require("LuaLib")
require("TutorialLib")

ui.loadcatalog("ComboSystemTutorial")
ui.loadcatalog("MoveNames")

local routine
local state
local runAI



-- ##########################################
-- Combo GO #################################
-- ##########################################

-- jLP airballH(214HK) air dash cancel
-- 5HP
-- jHK air dash cancel
-- jLK jHP jHK
-- 2MP 5MK 5HP
-- jMK air dash cancel 
-- jLP jLK
-- 5LP 5LP 5LK 5MP 2MK 2HP 
-- hairballH(214HK) Fenrir(623PP)


local Combo 
Combo = {
  wantLoadState = false,
  did1 = false,
  did2 = false,
  did3 = false,
  did4 = false,
  did5 = false,
  did6 = false,
  did7 = false,
  did8 = false,
  did9 = false,
  {
    text = "CS_Filia1Trial7_pt1",  --jLP airballH(214HK) air dash cancel
   -- details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()

      if Combo.wantLoadState then
        savedState()
        wait(25)
        Combo.wantLoadState = false
      end
      
      local success = false
      local wedidit = false

      local hitwithAirBall = false
      local hitwithJLP = false
      local DoAirDash = false

      while not success do

        if not p2:hasTag("Hitstun") then
          hitwithJLP = false
          hitwithAirBall = false
          DoAirDash = false
        end

        if p1:hasTag("j.WP") and p2:hasTag("Hitstun") then
          hitwithJLP = true
        end
        if p1:hasTag("Hairball") and p1.Slot6 == 2 and p1.grd == 0 and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithAirBall = true
        end
        if p1:hasTag("AirDash") then
          DoAirDash = true
        end

        if hitwithJLP and hitwithAirBall and DoAirDash then
          success = true
          wedidit = true
          break
        end

        wait()

      end

      if wedidit then
        helpers.success()
        did1 = true
      end

    end
  },
  {
    text = "CS_Filia1Trial7_pt2",  -- 5HP
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithSHP = false
    
      while not success do

        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("s.HP") and p2:hasTag("Hitstun") then
          hitwithSHP = true
        end

        if hitwithSHP and did1 then
          success = true
          wedidit = true
          break
        end

        wait()

      end

      if wedidit then
        helpers.success()
        did2 = true
      else
        helpers.fail()
        did1 = false
        did2 = false
        Combo.wantLoadState = true
      end

    end
  },
  {
    text = "CS_Filia1Trial7_pt3",  -- jHK air dash cancel
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()
      local success = false
      local wedidit = false

      local hitwithJHK = false
      local DoAirDash = false

      while not success do
        
        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("j.HK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithJHK = true
        end
        if p1:hasTag("AirDash") then
          DoAirDash = true
        end

        if hitwithJHK and DoAirDash and did2 then
          success = true
          wedidit = true
          break
        end
        wait()

      end
      
      if wedidit then
        helpers.success()
        did3 = true
      else
        helpers.fail()
        did1 = false
        did2 = false
        did3 = false
        Combo.wantLoadState = true
      end

    end
  },
  {
    text = "CS_Filia1Trial7_pt4",  -- jLK jHP jHK
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithJLK = false
      local hitwithJHP = false
      local hitwithJHK = false

      while not success do

        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("j.WK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithJLK = true
        end
        if p1:hasTag("j.HP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithJHP = true
        end
        if p1:hasTag("j.HK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithJHK = true
        end

        if hitwithJLK and hitwithJHP and hitwithJHK and did3 then
          success = true
          wedidit = true
          break
        end
        wait()

      end

      if wedidit then
        helpers.success()
        did4 = true
      else
        helpers.fail()
        did1 = false
        did2 = false
        did3 = false
        did4 = false
        Combo.wantLoadState = true
      end

    end
  },
  {
    text = "CS_Filia1Trial7_pt5",  -- 2MP 5MK 5HP
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithCMP = false
      local hitwithSMK = false
      local hitwithSHP = false

      while not success do

        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("c.MP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithCMP = true
        end
        if p1:hasTag("s.MK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithSMK = true
        end
        if p1:hasTag("s.HP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithSHP = true
        end

        if hitwithCMP and hitwithSMK and hitwithSHP and did4 then
          success = true
          wedidit = true
          break
        end

        wait()

      end

      if wedidit then
        helpers.success()
        did5 = true
      else
        helpers.fail()
        did1 = false
        did2 = false
        did3 = false
        did4 = false
        did5 = false
        Combo.wantLoadState = true
      end

    end
  },
  {
    text = "CS_Filia1Trial7_pt6",  -- jMK air dash cancel 
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithJMK = false
      local DoAirDash = false

      while not success do
        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("j.MK") and p1.numhits > 1 and p2:hasTag("Hitstun") then --p1.numhits > 1 - 2 Hits!!!
          hitwithJMK = true
        end
        if p1:hasTag("AirDash") then
          DoAirDash = true
        end

        if hitwithJMK and DoAirDash and did5 then
          success = true
          wedidit = true
          break
        end

        wait()

      end

      if wedidit then
        helpers.success()
        did6 = true
      else
        helpers.fail()
        did1 = false
        did2 = false
        did3 = false
        did4 = false
        did5 = false
        did6 = false
        Combo.wantLoadState = true
      end

    end
  },
  {
    text = "CS_Filia1Trial7_pt7",  -- jLP jLK
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()
      
      local success = false
      local wedidit = false

      local hitwithJLP = false
      local hitwithJLK = false

      while not success do
        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("j.WP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithJLP = true
        end
        if p1:hasTag("j.WK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithJLK = true
        end

        if hitwithJLP and hitwithJLK and did6 then
          success = true
          wedidit = true
          break
        end

        wait()

      end

      if wedidit then
        helpers.success()
        did7 = true
      else
        helpers.fail()
        did1 = false
        did2 = false
        did3 = false
        did4 = false
        did5 = false
        did6 = false
        did7 = false
        Combo.wantLoadState = true
      end

    end
  },
  {
    text = "CS_Filia1Trial7_pt8",  -- 5LP 5LP 5LK 5MP 2MK 2HP 
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()
      
      local success = false
      local wedidit = false

      local hitwithSLP = false
      local hitwithSLK = false
      local hitwithSMP = false
      local hitwithCMK = false
      local hitwithCHP = false

      while not success do
        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("s.WP2") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithSLP = true
        end
        if p1:hasTag("s.WK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithSLK = true
        end
        if p1:hasTag("s.MP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithSMP = true
        end
        if p1:hasTag("c.MK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithCMK = true
        end
        if p1:hasTag("c.HP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithCHP = true
        end

        if hitwithSLP and hitwithSLK and hitwithSMP and hitwithCMK and hitwithCHP and did7 then
          success = true
          wedidit = true
          break
        end

        wait()

      end

      if wedidit then
        helpers.success()
        did8 = true
      else
        helpers.fail()
        did1 = false
        did2 = false
        did3 = false
        did4 = false
        did5 = false
        did6 = false
        did7 = false
        did8 = false
        Combo.wantLoadState = true
      end

    end
  },
  {
    text = "CS_Filia1Trial7_pt9",  -- hairballH(214HK) Fenrir(623PP)
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()
      
      local success = false
      local wedidit = false

      local didHairball = false
      local hitwithFenrir = false

      while not success do
        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("Hairball") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          didHairball = true
        end
        if p1:hasTag("Fenrir") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithFenrir = true
        end

        if didHairball and hitwithFenrir and did8 then
          success = true
          wedidit = true
          break
        end

        wait()

      end

      if wedidit then
        helpers.success()
        did9 = true
      else
        helpers.fail()
        did1 = false
        did2 = false
        did3 = false
        did4 = false
        did5 = false
        did6 = false
        did7 = false
        did8 = false
        did9 = false
        Combo.wantLoadState = true
      end

    end
  }
}

Combo.reload = false
Combo.init = function()
  local p1, p2 = getP1(), getP2()
  savedState = saveState()
  p1:addForbiddenTag("")
  runAI = true
  p2:blockingAllowed(false)
  routine = coroutine.create(function() AIfunc() end)
  --routine = coroutine.create(function() while true do wait() end end)
end
Combo.deinit = function()
  getP1():clearForbiddenTags()
  routine = coroutine.create(function() while true do wait() end end)
  runAI = false
  standardwait()
end
Combo.type = "multi"

--- AI ROUTES! ###################################################

function AIfunc()
  local p1, p2 = getP1(), getP2()
  while true do
    if runAI then
      if p2:hasTag("Hitstun") then
        doMotion("F+Jab")
        coroutine.yield()
      else              
          coroutine.yield()
      end
    else
      wait()
      coroutine.yield()
    end
  end
end

--- Make the tutorial go! ####################################



local tutorialsections = {
  Combo,
}

setInit(function()
  dosections(tutorialsections)

  setTimer(function()
    state = saveState()
  end, 1)
end)

setUpdate(function()
  if type(routine) == "thread" and coroutine.status(routine) == "suspended" then
    coroutine.resume(routine)
  end
end)
