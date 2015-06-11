from .. import map_info
from ..basic import *
from ..command import *
from .. import event

def makeMapTypes(row, col):
    """Create a map like this
    ............
    ............
    ....****....
    ....****....
    ............
    ............
    """
    types = [[0] * col for i in xrange(row)]  # Empty map

    start_row = row / 3
    end_row = start_row * 2
    start_col = col / 3
    end_col = start_col * 2

    for r in xrange(start_row, end_row):
        for c in xrange(start_col, end_col):
            types[r][c] = 1

    return types


def makeMap():
    m = map_info.MapInfo(90, 90,
                         max_population=100,
                         max_round=100,
                         record_interval=1,
                         time_per_round=1.0,
                         weather=0,
                         types=makeMapTypes(90, 90))
    eles = {}
    eles['base0'] = Base(0, Rectangle(Position(30, 30),
                                      Position(32, 32)))
    eles['base1'] = Base(1, Rectangle(Position(57, 57),
                                      Position(59, 59)))
    eles['fort0'] = Fort(2, Rectangle(Position(41, 41),
                                      Position(42, 42)))
    eles['fort1'] = Fort(2, Rectangle(Position(47, 47),
                                      Position(48, 48)))

    eles['mine0'] = Mine(Position(36, 43))
    eles['oilfield0'] = Mine(Position(36, 46))
    eles['mine1'] = Mine(Position(53, 46))
    eles['oilfield1'] = Mine(Position(53, 43))

    for e in eles.values():
        m.add_element(e)

    return m


if __name__ == '__main__':
    makeMap().save('/Users/lisihan969/src/projects/teamstyle16/src/deep_blue/maps/island.map')
