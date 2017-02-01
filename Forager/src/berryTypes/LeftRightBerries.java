package berryTypes;

import foraging.ForagerScenario;

import static foraging.Map.BERRY;
import static foraging.Map.EMPTY;

/**
 * Created by david on 20-09-2016.
 */
public class LeftRightBerries implements BerryType {
    @Override
    public void fillBerries(byte[][] map, int numberOfBerries) {
        int counter=0;
        for (int i = 0; i < numberOfBerries;counter++) {
            map[1+counter][1] |= BERRY;
            i++;
            if(i<numberOfBerries) {
                map[ForagerScenario.mapX - 1 - 1 - counter][1] |= BERRY;
                i++;
            }

        }
    }
}
