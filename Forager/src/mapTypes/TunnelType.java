package mapTypes;

import foraging.Map;

import static foraging.Map.BASE;
import static foraging.Map.OBSTACLE;

/**
 * Created by david on 20-09-2016.
 */
public class TunnelType implements MapType  {
    @Override
    public void fillObstacles(byte[][] map) {
        int x=map.length;
        int y=map[0].length;

        for (int i = 0; i < x; i++) {
            map[i][0] = OBSTACLE;
            map[i][y - 1] = OBSTACLE;
        }
        for (int j = 0; j < y; j++) {
            map[0][j] = OBSTACLE;
            map[x - 1][j] = OBSTACLE;
        }

        // 10% of heigh as obstacle wall in middle of map, with 1-width gap
        for (int j = y / 2 - y / 10; j <= y / 2 + y / 10; j++) {
            for (int i = 0; i < x / 2; i++) {
                map[i][j] = OBSTACLE;
                map[x - 1 - i][j] = OBSTACLE;
            }
        }
        for (int i = x / 2 - x / 10; i <= x / 2 + x / 10; i++) {
            map[i][y - 2] = BASE;
        }
    }
}
