Nov. 9th 24
    Sometimes when a ball collides with the player and the player is stationary, the player and/or ball bounce off in wrong
    directions. This is better than what happened before, where player didn't move at all if stationary. This was fixed by
    checking to see if the player's velocity was between -1 and 1, and if so the math wouldn't work right, so I give
    player a new velocity above zero. Also, need to figure out how to stop player velocity from going so low.

    still some clipping issues

    next I want to add shooting