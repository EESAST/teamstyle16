from .. import gamebody
from .. import map_info
from ..basic import *
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

        # bases, assign ranges explicitly to avoid changes in basic.py
        self.base0 = Base(0, Rectangle(Position(0, 0, 1),
                                       Position(2, 2, 1)),
                          sight_ranges=[4, 10, 8],
                          fire_ranges=[0, 7, 5])
        self.base1 = Base(1, Rectangle(Position(12, 12, 1),
                                       Position(14, 14, 1)),
                     sight_ranges=[4, 10, 8],
                     fire_ranges=[0, 7, 5])
        self.assertTrue(self.gamebody.map_info.add_element(self.base0))
        self.assertTrue(self.gamebody.map_info.add_element(self.base1))
        # forts
        self.fort0 = Fort(2, Position(9, 12, 1))
        self.fort1 = Fort(2, Rectangle(Position(6, 6, 1),
                                       Position(7, 7, 1)))
        self.fort2 = Fort(2, Position(9, 4, 1))
        self.assertTrue(self.gamebody.map_info.add_element(self.fort0))
        self.assertTrue(self.gamebody.map_info.add_element(self.fort1))
        self.assertTrue(self.gamebody.map_info.add_element(self.fort2))

        # oilfield
        self.oilfield0 = Oilfield(Position(13, 2, 1))
        self.oilfield1 = Oilfield(Position(3, 7, 1))
        self.assertTrue(self.gamebody.map_info.add_element(self.oilfield0))
        self.assertTrue(self.gamebody.map_info.add_element(self.oilfield1))

        # mine
        self.mine0 = Mine(Position(14, 5, 1))
        self.mine1 = Mine(Position(3, 10, 1))
        self.assertTrue(self.gamebody.map_info.add_element(self.mine0))
        self.assertTrue(self.gamebody.map_info.add_element(self.mine1))

    def test_constants(self):
        """Test constants in module gamebody"""
        cont = gamebody.STATE_CONTINUE
        tie = gamebody.STATE_TIE
        self.assertNotEqual(cont, 0)
        self.assertNotEqual(cont, 1)
        self.assertNotEqual(tie, 0)
        self.assertNotEqual(tie, 1)
        self.assertNotEqual(cont, tie)

    def test_basic_view(self):
        """Test baisc behavior of view"""
        units_in_sight = self.gamebody.view_elements(1)
        for element in [self.base1, self.oilfield0, self.mine0]:
            self.assertIn(element, units_in_sight.values())

    def test_view_outrange_base(self):
        """Test behavior of viewing a base out of sight"""
        # units_in_sight = self.gamebody.view_elements(1)
        # self.assertIn(self.base0, units_in_sight)
        pass

    def test_view_outrange_resource(self):
        """Test behavior of viewing a base out of sight (see nothing)"""
        # units_in_sight = self.gamebody.view_elements(1)
        # self.assertNotIn(self.oilfield1, units_in_sight)
        # self.assertNotIn(self.mine1, units_in_sight)
        pass
):

    # def test_attack_pos(self):
    #     """Test behavior of attack position"""
    #     self.assertTrue(self.gamebody.map_info.add_element(
    #         Destroyer(1, Position(5, 2, 1), sight_ranges=[1, 3, 2],
    #                                         fire_ranges=[1, 3, 2])))
    # def test_attck_unit(self):
    #     """Test behavior of attack unit"""
    #     pass

    # def test_change_dest(self):
    #     """Test behavior of change destination"""
    #     pass

    # def test_fix(self):
    #     """Test behavior of fix"""
    #     pass

    # def test_produce(self):
    #     """Test behavior of produce"""
    #     pass

    # def test_supply(self):
    #     """Test behavior of supply"""
    #     pass

    # def test_cancel(self):
    #     """Test behavior of cancel"""
    #     pass

    def test_ignore_invalid_commands(self):
        """Test whether gamebody will ignore invalid commands"""
        #test AttackPos
            #无效情况1：弹药量少于单次攻击所需弹药量
            #无效情况2：攻击目标不在发出攻击的单位攻击范围内

        #test AttackUnit
            #无效情况1：弹药量少于单次攻击所需弹药量
            #无效情况2：攻击目标不在发出攻击的单位攻击范围内
            #无效情况3：攻击目标是己方单位
            #无效情况4：攻击目标不在攻击发出的单位视野范围内

        #test Fix
            #无效情况1：维修指令发出的单位不是基地
            #无效情况2：维修单位不在基地的维修范围内
            #无效情况3：基地金属不足

        #test ChangeDes
            #无效情况1：新更改的坐标越地图边界
            #无效情况2：单位无法到新更改的坐标上
            #无效情况3：新坐标上已存在其他单位（这个算吗？）

        #test Produce
            #无效情况1：金属不足
            #无效情况2：总人口数超过上限

        #test Supply
            #无效情况1：发出补给指令的单位没有不及权限
            #无效情况2：被补给的对象不在补给指令发出方的补给范围内
        sub = Submarine(team=1, pos=Position(4, 4, 0), fire_ranges=[1, 2, 3])
        index = self.gamebody.map_info.add_element(sub)
        assertFalse(self.gamebody.set_command(command.AttackUnit(index, self.base0.index)))
        

    def test_exclude_commands(self):
        """Test whether the newer commands will recover the older ones conflicting with them"""
        pass

    def test_cancle(self):
        """Test whether it will stop all the commands related to this unit"""
        pass

if __name__ == '__main__':
    unittest.main()
