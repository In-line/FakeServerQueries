#if defined _FakeServerQueries_included
	#endinput
#endif
#define _FakeServerQueries_included

#include <amxmodx>

#pragma reqlib fsq
#if !defined AMXMODX_NOAUTOLOAD
	#pragma loadlib fsq
#endif 

#define FSQ_ERROR -1

/*
* Sets players diff. Final player count is calculated like this fakePlayers + realPlayers + playersDiff
*
* @param playersDiff    players diff in range [-127, 127]
*
* @return               FSQ_ERROR on error.
* @error                If players diff is out of range an error will be thrown.
*/
native fsq_set_players_diff(const playersDiff = 0);

/* 
* Creates new fake player
*
* @param name                       Displayed name
* @param score                      Displayed score
* @param duation                    Displayed duration, automatically incremented
* @param random_duration_increment  Adds some random time to duration in range (0, 4.0)
*
* @return                           Fake player id (FSQ_ERROR on error)
* @error                            If fake player id is invalid an error will be thrown.
*/
native fsq_create_fake(name[], score = 0, Float:duration = 0.0, bool:random_duration_increment = true);

/*
* Destroyes fake player. 
*
* @param id     Fake player id
*
* @return       FSQ_ERROR on error
* @error        If fake player id is invalid an error will be thrown.
*/
native fsq_destroy_fake(id);

/*
* Get duration of the fake.
*
* @param id     Fake player id
*
* @return       Duration (FSQ_ERROR on error)
* @error        If fake player id is invalid an error will be thrown.
*/
native Float:fsq_get_fake_duration(id);

/*
* Set duration of the fake.
*
* @param id     		    Fake player id
* @param duation                    Displayed duration, automatically incremented
* @param random_duration_increment  Adds some random time to duration in range (0, 4.0)
*
* @return       Duration
* @error        If fake player id is invalid an error will be thrown.
*/
native fsq_set_fake_duration(id, Float:duration, bool:random_duration_increment = true);

/*
* Get score of the fake.
*
* @param id     Fake player id
*
* @return       Score  (FSQ_ERROR on error)
* @error        If fake player id is invalid an error will be thrown.
*/
native fsq_get_fake_score(id);

/*
* Set duration of the fake.
*
* @param id     Fake player id
* @param score  Displayed score
*
* @return       Score  (FSQ_ERROR on error)
* @error        If fake player id is invalid an error will be thrown.
*/
native fsq_set_fake_score(id, score);
