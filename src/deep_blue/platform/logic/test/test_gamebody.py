from .. import gamebody
from .. import map_info
from ..basic import *
from ..command import *
from .. import event

import unittest

class TestGameBody(unittest.TestCase):
    """Test case for module gamebody"""

    def setUp(self):
        """Build a gamebody for testing"""
        # a 15 * 15 map
        types = [[1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0],
                 [1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0],
                 [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1],
                 [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1],
                 [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1],
                 [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1],
                 [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1]]
        m = map_info.MapInfo(15, 15,
                             max_population=100,
                             max_round=100,
                             record_interval=1,
                             time_per_round=1.0,
                             weather=0,
                             types=types)
        self.gamebody = gamebody.GameBody(m)
        m = self.gamebody.map_info

        # bases, assign ranges explicitly to avoid changes in basic.py
        self.base0 = Base(0, Rectangle(Position(0, 0, 1),
                                       Position(2, 2, 1)),
                          sight_ranges=[4, 10, 8],
                          fire_ranges=[0, 7, 5])
        self.base1 = Base(1, Rectangle(Position(12, 12, 1),
                                       Position(14, 14, 1)),
                          sight_ranges=[4, 10, 8],
                          fire_ranges=[0, 7, 5])
        self.assertIsNotNone(m.add_element(self.base0))
        self.assertIsNotNone(m.add_element(self.base1))
        # forts
        self.fort0 = Fort(1, Position(9, 12, 1), sight_ranges=[2, 2, 2])
        self.fort1 = Fort(2, Rectangle(Position(6, 6, 1),
                                       Position(7, 7, 1)))
        self.fort2 = Fort(2, Position(9, 4, 1))
        self.assertIsNotNone(m.add_element(self.fort0))
        self.assertIsNotNone(m.add_element(self.fort1))
        self.assertIsNotNone(m.add_element(self.fort2))

        # oilfield
        self.oilfield0 = Oilfield(Position(13, 2, 1))
        self.oilfield1 = Oilfield(Position(3, 7, 1))
        self.assertIsNotNone(m.add_element(self.oilfield0))
        self.assertIsNotNone(m.add_element(self.oilfield1))

        # mine
        self.mine0 = Mine(Position(14, 5, 1))
        self.mine1 = Mine(Position(3, 10, 1))
        self.assertIsNotNone(m.add_element(self.mine0))
        self.assertIsNotNone(m.add_element(self.mine1))

    def add(self, cls, team, pos, **kw):
        """Helper method, add element into the map"""
        element = cls(team, Position(pos[0], pos[1], 0), **kw)
        self.assertIsNotNone(self.gamebody.map_info.add_element(element))
        return element

    def varify_attrs(self, element, ghost, pub_attrs, priv_attrs):
        for attr in pub_attrs:
            if hasattr(element, attr):
                # Should be visible to everyone
                self.assertEqual(getattr(element, attr),
                                 getattr(ghost, attr))
        for attr in priv_attrs:
            if hasattr(element, attr):
                # Should not be visible to enemy
                if attr == 'pos' or attr == 'dest':
                    # pos/dest should be set to None
                    expected = None
                elif attr == 'team':
                    expected = 2
                else:
                    expected = 0
                self.assertEqual(expected, getattr(ghost, attr))

    def test_constants(self):
        """Test constants in module gamebody"""
        cont = gamebody.STATE_CONTINUE
        tie = gamebody.STATE_TIE
        self.assertNotEqual(cont, 0)
        self.assertNotEqual(cont, 1)
        self.assertNotEqual(tie, 0)
        self.assertNotEqual(tie, 1)
        self.assertNotEqual(cont, tie)

    def test_view_friendly(self):
        """Test behavior of viewing friendly (see everything)"""
        friendly1 = self.add(Fighter, 1, (14, 14))
        friendly2 = self.add(Carrier, 1, (8, 5))

        units_in_sight = self.gamebody.view_elements(1)
        for element in [self.base1, self.fort0, friendly1, friendly2]:
            self.assertIn(element, units_in_sight.values())

    def test_view_insight_enemy(self):
        """Test behavior of viewing enemies in sight (no fuel, ammo, metal,
        dest)"""
        self.add(Scout, 1, (0, 2))  # light up enemy base
        self.fort1.team = 0  # Create an enemy fort
        enemy1 = self.add(Fighter, 0, (14, 4), dest=Position(1, 1, 2))
        enemy2 = self.add(Destroyer, 0, (11, 13))
        enemy3 = self.add(Submarine, 0, (8, 12), dest=Position(8, 11, 0))

        units_in_sight = self.gamebody.view_elements(1)
        for element in [self.base0, self.fort1, enemy1, enemy2, enemy3]:
            self.assertIn(element.index, units_in_sight)
            ghost = units_in_sight[element.index]

            pub_attrs = ['kind', 'position', 'size', 'team', 'health']
            priv_attrs = ['fuel', 'ammo', 'metal', 'dest']

            self.varify_attrs(element, ghost, pub_attrs, priv_attrs)

    def test_view_outsight_enemy_unit(self):
        """Test behavior of viewing enemy units out of sight (see nothing)"""
        enemy1 = self.add(Fighter, 0, (13, 3))
        enemy2 = self.add(Destroyer, 0, (5, 8))
        enemy3 = self.add(Submarine, 0, (7, 14))

        units_in_sight = self.gamebody.view_elements(1)
        for element in [enemy1, enemy2, enemy3]:
            self.assertNotIn(element.index, units_in_sight)

    def test_view_outsight_fort(self):
        """Test behavior of viewing forts out of sight (only pos)"""
        for team in [0, 2]:
            self.fort2.team = team  # Create an enemy/neutral fort

            units_in_sight = self.gamebody.view_elements(1)
            self.assertIn(self.fort2.index, units_in_sight)
            ghost = units_in_sight[self.fort2.index]

            pub_attrs = ['kind', 'position', 'size']
            priv_attrs = ['team', 'health', 'fuel', 'ammo', 'metal']

            self.varify_attrs(self.fort2, ghost, pub_attrs, priv_attrs)

    def test_view_outrange_resource(self):
        """Test behavior of viewing resources out of sight (see only pos)"""
        units_in_sight = self.gamebody.view_elements(1)

        pub_attrs = ['kind', 'position', 'size']
        priv_attrs = ['fuel', 'metal']

        for element in [self.mine1, self.oilfield1]:
            self.assertIn(element.index, units_in_sight)
            ghost = units_in_sight[element.index]
            self.varify_attrs(element, ghost, pub_attrs, priv_attrs)

    def test_attack_pos(self):
        """Test behavior of attack position"""
        # 1. Normal attack
        # Add attcker & defencer
        attacker = self.add(Destroyer, 1, (5, 2), fire_ranges=[99, 5, 99],
                                                  attacks=[20, 10],
                                                  ammo_max=200,
                                                  ammo_once=5)
        defencer = self.add(Destroyer, 0, (6, 2), defences=[10, 5],
                                                  health_max=100)

        def attack_pos(index, pos):
            # Set command and run
            cmd = AttackPos(index, pos)
            self.assertTrue(self.gamebody.set_command(1, cmd))
            return self.gamebody.run()

        results = attack_pos(attacker.index, defencer.pos)
        # damage = (20 * 7/6 - 10) + (10 * 7/6 - 5) = 19
        self.assertEqual(81, defencer.health)
        self.assertEqual(195, attacker.ammo)

        # 2. Defences greater than attacks
        defencer.defences = [30, 8]
        results = attack_pos(attacker.index, defencer.pos)
        # damage = 10 * 7/6 - 8 = 3
        self.assertEqual(78, defencer.health)
        self.assertEqual(190, attacker.ammo)

        # 3. Defences is infinity
        defencer.defences = [INFINITY, 7]
        results = attack_pos(attacker.index, defencer.pos)
        # damage = 10 * 7/6 - 7 = 4
        self.assertEqual(74, defencer.health)
        self.assertEqual(185, attacker.ammo)

        # 4. Attack an empty position
        results = attack_pos(attacker.index, Position(6, 2, 0))
        # No damage
        self.assertEqual(74, defencer.health)
        self.assertEqual(180, attacker.ammo)

    def test_attack_unit(self):
        """Test behavior of attack unit"""
        # Just test the basic behavior
        attacker = self.add(Destroyer, 1, (5, 2), fire_ranges=[99, 5, 99],
                                                  attacks=[20, 10],
                                                  ammo_max=200,
                                                  ammo_once=5)
        defencer = self.add(Destroyer, 0, (6, 2), defences=[10, 30],
                                                  health_max=100)

        # Set command and run
        cmd = AttackUnit(attacker.index, defencer.index)
        self.assertTrue(self.gamebody.set_command(1, cmd))
        results = self.gamebody.run()
        # damage = 20 * 7/6 - 10 = 13
        self.assertEqual(87, defencer.health)
        self.assertEqual(195, attacker.ammo)

    def test_change_dest(self):
        """Test behavior of change destination"""
        unit = self.add(Scout, 1, (2, 2), dest=Position(14, 12, 2))
        new_dest = Position(10, 5, 2)

        cmd = ChangeDest(unit.index, new_dest)
        self.assertTrue(self.gamebody.set_command(1, cmd))
        results = self.gamebody.run()

        self.assertEqual(new_dest, unit.dest)

    def test_collect(self):
        """Test behavior of collect (auto collect when distance == 1 and stop,
        only Cargo)"""
        hungry_state = {
            'health_max': 100, 'health': 10,
            'fuel_max': 200, 'fuel': 50,
            'ammo_max': 300, 'ammo': 0,
            'metal_max': 40, 'metal': 30
        }
        cargo1 = self.add(Cargo, 0, (12, 2), **hungry_state)
        cargo2 = self.add(Cargo, 0, (14, 6), **hungry_state)
        cargo3 = self.add(Cargo, 0, (2, 6), dest=Position(2, 8, 1), speed=3, **hungry_state)
        cargo4 = self.add(Cargo, 0, (0, 10), dest=Position(2, 10, 1), speed=3, **hungry_state)
        self.oilfield0.fuel = 10
        self.oilfield1.fuel = 500
        self.mine0.metal = 100
        self.mine1.metal = 0

        results = self.gamebody.run()
        self.assertEqual(4, len(results))

        expected = [event.Collect(cargo1.index, self.oilfield0.index, 10, 0),
                    event.Collect(cargo2.index, self.mine0.index, 0, 10)]
        results = {e.index: e for e in results}

        # Check events
        for e in expected:
            self.assertIn(e.index, results)
            result = results[e.index]
            for attr in ['target', 'fuel', 'metal']:
                self.assertEqual(getattr(e, attr), getattr(result, attr))

        # Check consequences
        self.assertEqual(10, cargo1.health)
        self.assertEqual(60, cargo1.fuel)
        self.assertEqual(0, cargo1.ammo)
        self.assertEqual(30, cargo1.metal)

        self.assertEqual(50, cargo2.fuel)
        self.assertEqual(40, cargo2.metal)

        self.assertEqual(0, self.oilfield0.fuel)
        self.assertEqual(90, self.mine0.metal)
        self.assertEqual(500, self.oilfield1.fuel)
        self.assertEqual(0, self.mine1.metal)

    def test_fix(self):
        """Test behavior of fix"""
        boat = self.add(Cargo, 0, (3, 2), health_max=100, health=10,
                                          fuel_max=200, fuel=50,
                                          ammo_max=300, ammo=0,
                                          metal_max=40, metal=30)
        plane = self.add(Fighter, 1, (12, 12), health_max=100, health=50,
                                               fuel_max=200, fuel=100,
                                               ammo_max=300, ammo=5)

        cmd0 = Fix(self.base0.index, boat.index)
        cmd1 = Fix(self.base1.index, plane.index)
        self.assertTrue(self.gamebody.set_command(0, cmd0))
        self.assertTrue(self.gamebody.set_command(1, cmd1))
        results = self.gamebody.run()

        # team 0
        for attr in ['health', 'fuel', 'ammo', 'metal']:
            self.assertEqual(getattr(boat, attr + '_max'), getattr(boat, attr))

        metal_cost = int(METAL_PER_HEALTH * 90 + 10)  # round down
        self.assertEqual(self.base0.fuel, self.base0.fuel_max - 150)
        self.assertEqual(INFINITY, self.base0.ammo)
        self.assertEqual(self.base0.metal, self.base0.metal_max - metal_cost)

        # team 1
        for attr in ['health', 'fuel', 'ammo']:
            self.assertEqual(getattr(plane, attr + '_max'), getattr(plane, attr))

        metal_cost = int(METAL_PER_HEALTH * 50)  # round down
        self.assertEqual(self.base1.fuel, self.base1.fuel_max - 100)
        self.assertEqual(INFINITY, self.base1.ammo)
        self.assertEqual(self.base1.metal, self.base1.metal_max - metal_cost)

    def test_produce(self):
        """Test behavior of produce"""
        game = self.gamebody
        build_round = PROPERTY[FIGHTER]['build_round']

        cmd = Produce(Fighter.kind)
        self.assertTrue(game.set_command(0, cmd))

        while build_round > 1:
            game.run()
            build_round -= 1
            self.assertEqual(game.production_list(0), [[Fighter.kind, build_round]])
        # one round left
        results = game.run()
        self.assertEqual(1, len(results))
        create = results[0]

        self.assertTrue(isinstance(create, event.Create))
        plane = game.map_info.elements[create.index]

        self.assertEqual(Fighter.kind, create.kind)
        self.assertTrue(isinstance(plane, Fighter))
        self.assertEqual(plane.pos, create.pos)
        self.assertEqual(0, plane.pos.distance(self.base0.pos))

    def test_supply(self):
        """Test behavior of supply"""
        # Building, Carrier, Cargo (diffrent when supply planes)
        pass

    def test_cancel(self):
        """Test behavior of cancel"""
        attacker = self.add(Destroyer, 1, (5, 2))
        defencer = self.add(Destroyer, 0, (6, 2))

        cmd = AttackUnit(attacker.index, defencer.index)
        self.assertTrue(self.gamebody.set_command(1, cmd))
        self.assertEqual(1, len(self.gamebody.commands[1]))

        cmd = Cancel(attacker.index)
        self.assertTrue(self.gamebody.set_command(1, cmd))
        self.assertEqual(0, len(self.gamebody.commands[1]))

if __name__ == '__main__':
    unittest.main()
