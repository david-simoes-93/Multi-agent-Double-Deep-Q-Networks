package berryTypes;

import foraging.ForagerScenario;

import static foraging.Map.BERRY;
import static foraging.Map.EMPTY;

/**
 * Created by david on 20-09-2016.
 */
public class RandomBerries implements BerryType {
    @Override
    public void fillBerries(byte[][] map, int numberOfBerries) {
        for (int i = 0; i < numberOfBerries; i++) {
            while (!placeBerry(map));
        }
    }

    public boolean placeBerry(byte [][] map) {
        int x = (int) (Math.random() * map.length);
        int y = (int) (Math.random() * 4 * map[0].length / 10.0);

        if (map[x][y] == EMPTY) {
            map[x][y] |= BERRY;
            return true;
        }
        return false;
    }

}
