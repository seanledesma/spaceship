Nov. 9th 24
    Sometimes when a ball collides with the player and the player is stationary, the player and/or ball bounce off in wrong
    directions. This is better than what happened before, where player didn't move at all if stationary. This was fixed by
    checking to see if the player's velocity was between -1 and 1, and if so the math wouldn't work right, so I give
    player a new velocity above zero. Also, need to figure out how to stop player velocity from going so low.

    still some clipping issues

    next I want to add shooting
    how will i go about getting shooting to work? I suppose I could create another array of circles,
    and add another check to see if those hit an asteroid or not. Would the array need to resize itself? Could the player basically shoot an infinite number of balls? because if so, that's a lot to check against all the other balls. I could keep
    a smaller array, say the player only gets 10 shots before a cool down period and maybe show that on another bar, how
    many shots before reload. and if the bullet ball hits an asteroid, see if I can do more than just a regular transfer of
    power, to push the asteroid out of the way and not delete it entirely, but I would want the bullet ball to dissapear.