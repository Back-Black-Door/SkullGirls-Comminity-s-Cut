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

--2LK 5MK 5HP
--jHP jHK air dash cancel
--jLK jHP jHK
--2LK 2HP
--Ringlet Spike (L M or H)
--jLP airballH(214HK) dash cancel
--5LP 5LP 5LK 5MP 
--(Wait) 2MK 5HK 
--hairballH(214HK) Fenrir(623PP)


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
    text = "CS_Filia1Trial5_pt1",  --cLK sMK sHP 
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

      local hitwithCLK = false
      local hitwithSMK = false
      local hitwithSHP = false

      while not success do

        if not p2:hasTag("Hitstun") then
          hitwithCLK = false
          hitwithSMK = false
          hitwithSHP = false
        end

        if p1:hasTag("c.WK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithCLK = true
        end
        if p1:hasTag("s.MK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithSMK = true
        end
        if p1:hasTag("s.HP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithSHP = true
        end

        if hitwithCLK and hitwithSMK and hitwithSHP then
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
    text = "CS_Filia1Trial5_pt2",  --jHP jHK airdash cancel
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithJHP = false
      local hitwithJHK = false
      local DoAirDash = false

      

      while not success do

        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("j.HP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithJHP = true
        end
        if p1:hasTag("j.HK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithJHK = true
        end
        if p1:hasTag("AirDash") then
          DoAirDash = true
        end

        if hitwithJHP and hitwithJHK and DoAirDash and did1 then
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
    text = "CS_Filia1Trial5_pt3",  --jLK jHP jHK
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
        
        if hitwithJLK and hitwithJHP and hitwithJHK and did2 then
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
    text = "CS_Filia1Trial5_pt4",  --cLK cHP
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithCLK = false
      local hitwithCHP = false

      while not success do

        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("c.WK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithCLK = true
        end
        if p1:hasTag("c.HP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithCHP = true
        end

        if hitwithCLK and hitwithCHP and did3 then
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
    text = "CS_Filia1Trial5_pt5",  --Ringlet Spike (L M or H)
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithRingletSpike = false

      while not success do

        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("RingletSpike") and p2:hasTag("Hitstun") then
          hitwithRingletSpike = true
        end

        if hitwithRingletSpike and did4 then
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
    text = "CS_Filia1Trial5_pt6",  --jLP airballH(214HK) dash cancel
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithJLP = false
      local hitwithHairball = false
      local DoAirDash = false

      while not success do
        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("j.WP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithJLP = true
        end
        if p1:hasTag("Hairball") and p1.Slot6 == 2 and p1.grd == 0 and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithHairball = true
        end
        if p1:hasTag("AirDash") then
          DoAirDash = true
        end

        if hitwithJLP and hitwithHairball and DoAirDash and did5 then
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
    text = "CS_Filia1Trial5_pt7",  --5LP 5LP 5LK 5MP
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()
      
      local success = false
      local wedidit = false

      local hitwithSLP = false
      local hitwithSLK = false
      local hitwithSMP = false

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

        if hitwithSLP and hitwithSLK and hitwithSMP and did6 then
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
    text = "CS_Filia1Trial5_pt8",  --(Wait) 2MK 5HK
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()
      
      local success = false
      local wedidit = false

      local hitwithCMK = false
      local hitwithSHK = false

      while not success do
        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("c.MK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithCMK = true
        end
        if p1:hasTag("s.HK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithSHK = true
        end

        if hitwithCMK and hitwithSHK and did7 then
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
    text = "CS_Filia1Trial5_pt9",  --Hairball(214HK) -- > Fenrir(623PP)
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
