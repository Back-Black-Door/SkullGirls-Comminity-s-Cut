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
--jHK air dash cancel
--jLK jHP jHK
--2MP 5MK 5HP
--jMK airballH(214HK) air dash cancel
--jMP Restand
--5LP 5LP 5LK 5MP 
--(wait) 2MK 2HP 
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
    text = "CS_Filia1Trial4_pt1",  --cLK sMK sHP 
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
    text = "CS_Filia1Trial4_pt2",  --jHK air dash cancel
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

        if hitwithJHK and DoAirDash and did1 then
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
    text = "CS_Filia1Trial4_pt3",  --jLK jHP jHK
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
    text = "CS_Filia1Trial4_pt4",  --cMP sMK sHP
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

        if hitwithCMP and hitwithSMK and hitwithSHP and did3 then
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
    text = "CS_Filia1Trial4_pt5",  --jMK (2 hits) airballH(214HK) air dash cancel 
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithJMK = false
      local hitwithHairball = false
      local DoAirDash = false
      
      while not success do

        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("j.MK") and p1.numhits > 1 and p2:hasTag("Hitstun") then
          hitwithJMK = true
        end
        if p1:hasTag("Hairball") and p1.Slot6 == 2 and p1.grd == 0 and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithHairball = true
        end
        if p1:hasTag("AirDash") then
          DoAirDash = true
        end

        if hitwithJMK and hitwithHairball and DoAirDash and did4 then
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
    text = "CS_Filia1Trial4_pt6",  --jMP
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()

      local success = false
      local wedidit = false

      local hitwithJMP = false
      local restand = false

      while not success do
        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("j.MP") and p1.numhits > 0 and p2:hasTag("Hitstun") then -- (p2.ypos == 64) - restand!
          hitwithJMP = true
        end

        if hitwithJMP and did5 then
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
    text = "CS_Filia1Trial4_pt7",  --5LP 5LP 5LK 5MP
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
    text = "CS_Filia1Trial4_pt8",  --(wait) 2MK 2HP 
    --details = "DoTheCombo_Cmd",
    func = function(helpers)
      local p1, p2 = getP1(), getP2()
      
      local success = false
      local wedidit = false

      local hitwithCMK = false
      local hitwithCHP = false

      while not success do
        if not p2:hasTag("Hitstun") then
          wedidit = false
          break
        end

        if p1:hasTag("c.MK") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithCMK = true
        end
        if p1:hasTag("c.HP") and p1.numhits > 0 and p2:hasTag("Hitstun") then
          hitwithCHP = true
        end

        if hitwithCMK and hitwithCHP and did7 then
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
    text = "CS_Filia1Trial4_pt9",  --Hairball(214HK) -- > Fenrir(623PP)
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
  routine = coroutine.create(function() walkforthstand(200) end)
end
Combo.deinit = function()
  getP1():clearForbiddenTags()
  routine = coroutine.create(function() while true do wait() end end)
  runAI = false
  standardwait()
end
Combo.type = "multi"

--- AI ROUTES! ###################################################

function walkforthstand(len)
  local p1, p2 = getP1(), getP2()
  local dir = "f"
  len = len or 100
  while true do
    if runAI then
      if p2:hasTag("Hitstun") then
        doMotion("F+Jab")
        coroutine.yield()
      else       
      local diff = math.abs(p1.xpos - p2.xpos)
      if dir == "f" then
        if diff > len then
          addInput(d.f)         
          coroutine.yield()
        else
          dir = "n"
        end
      else
        if diff <= len then
          wait()
          coroutine.yield()
        else
          dir = "f"
        end
      end
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
