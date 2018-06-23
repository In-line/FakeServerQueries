/*
 * 
 * Copyright (c) 2018 Alik Aslanyan <cplusplus256@gmail.com>
 *
 *
 *    This file is part of Fake Server Queries
 *
 *    Fake Server Queries is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Fake Server Queries is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Fake Server Queries; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of Fake Server Queries with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#include <amxmodx>
#include <amxmisc>
#include <FakeServerQueries>
#include <cellarray>

#define PLUGIN "FakeServerQueries Manager"
#define VERSION "1.0"
#define AUTHOR "Inline"

#define RATIO 500
#define THINK_DELAY 1.0
#define RESERVE_SLOTS 1

#define SCORE_INCREMENT_CHANCE 0.5
#define SCORE_INCREMENT_DELAY 0.1

#define THINK_TASK 1
#define SCORE_INCREMENTER_TASK 2

new Array:g_FakesArray;
new Array:g_FakesNames;
new Array:g_FakesNamesIndices;

new g_MaxPlayers;
public plugin_init() {
	register_plugin(PLUGIN, VERSION, AUTHOR);
	
	g_FakesArray = ArrayCreate(1, 1);
	g_FakesNames = LoadNames();
	g_FakesNamesIndices = CreateShuffledNamesIndices(g_FakesNames);

	g_MaxPlayers = get_maxplayers();

	DeferThink(); 
	ScoreIncrementerTask();
}

public plugin_end() {
	ArrayDestroy(g_FakesArray);
	ArrayDestroy(g_FakesNames);
	ArrayDestroy(g_FakesNamesIndices);
}

Array:LoadNames() {
	new path[256];
	new configsDirLen = get_configsdir(path, charsmax(path));
	formatex(path[configsDirLen], charsmax(path) - configsDirLen, "/fsq_names.ini");
	if(!file_exists(path)) {
		log_to_error("File for bot names doesn't exist at path %s", path);
		return Invalid_Array;
	}

	new file = fopen(path, "r");
	if(!file) {
		log_to_error("Can't open name file at path %s", path);
		return Invalid_Array;
	}

	new Array:fakesNames = ArrayCreate(32, 1);
	
	new buffer[1024];
	new len = 0;
	while((len = fgets(file, buffer, charsmax(buffer))) != 0) {
		new commentPos = contain(buffer, "//");
		new alternativeCommentPos = contain(buffer, ";");
		if(alternativeCommentPos != -1) {
			if(commentPos == -1) { // No // comment found, set ; comment pos
				commentPos = alternativeCommentPos;
			} else { // Select the first one by position.
				commentPos = min(commentPos, alternativeCommentPos);
			}
		}

		if(commentPos != -1) {
			len -= commentPos;
			format(buffer, len, "%s", buffer[commentPos]); // Remove comment
		}
		trim(buffer);
		if(buffer[0] == '^0') {
			continue;
		}

		ArrayPushString(fakesNames, buffer);
	}

	if(ArraySize(fakesNames) == 0) {
		ArrayDestroy(fakesNames);
		return Invalid_Array;
	}

	return fakesNames;
}

Array:CreateShuffledNamesIndices(Array:names) {
	new size = ArraySize(names);
	new Array:indices = ArrayCreate(1, size);
	for(new i = 0; i < size;++i) {
		ArrayPushCell(indices, i);
	}
	for(new i = 0; i < size;++i) {
		ArraySwap(indices, i, random_num(0, size - 1));
	}

	return indices;
}

public GetNextName(outputName[32]) {
	static index = 0;
	if(index >= ArraySize(g_FakesNamesIndices)) {
		index = 0;
	}

	new realIndex = ArrayGetCell(g_FakesNamesIndices, index++);

	return ArrayGetString(g_FakesNames, realIndex, outputName, charsmax(outputName));
}

public client_connect(id) {
	TryToThinkNow();
}

public client_disconnected(id) {
	DeferThink();
}

TryToThinkNow() {
	new playersAndFakesNum = get_playersnum(true) + ArraySize(g_FakesArray) ;
	if(playersAndFakesNum >= g_MaxPlayers - RESERVE_SLOTS) {
		Think();
	} else  {
		DeferThink();
	}
}

DeferThink() {
	if(task_exists(THINK_TASK)) remove_task(THINK_TASK);
	set_task(THINK_DELAY, "Think", THINK_TASK);
}

AddFakes(num) {
	new name[32];
	for(new i = 0; i < num; ++i) {
		GetNextName(name);
		new fakeId = fsq_create_fake(name);
		if(fakeId == FSQ_ERROR) {
			continue;
		}
		ArrayPushCell(g_FakesArray, fakeId);
	
	}
}

RemoveFakes(num) {
	for(new i = 0; i < num; ++i) {
		 new fakesArraySize = ArraySize(g_FakesArray);
		 if(fakesArraySize == 0) {
		 	break;
		 }
		 
		new item = random_num(0, fakesArraySize - 1);
		new id = ArrayGetCell(g_FakesArray, item);
		fsq_destroy_fake(id);
		ArrayDeleteItem(g_FakesArray, item);
	}
}

stock log_to_error(const message[], any:...)
{
	new log[256], date[32];
	vformat(log, 255, message, 2);
	get_time("error_%Y%m%d.log", date, 31);
	log_to_file(date, "[%s] %s", PLUGIN, log);
}

public ScoreIncrementerTask() {
	new currentNumOfFakes = ArraySize(g_FakesArray); 
	if(currentNumOfFakes > 0 && random_num(0, 100) <= SCORE_INCREMENT_CHANCE) { // Increment score
		new item = random_num(0, currentNumOfFakes - 1);
		new fakeId = ArrayGetCell(g_FakesArray, item);
		if(fakeId != FSQ_ERROR) {
			fsq_set_fake_score(fakeId, fsq_get_fake_score(fakeId) + 1);
		}
	}
	set_task_ex(SCORE_INCREMENT_DELAY + random_float(0.0, 0.5), "ScoreIncrementerTask", SCORE_INCREMENTER_TASK);
}

public  Think() {

	new playersNum  = get_playersnum(true);	
	new fakesNum  = floatround(float(playersNum) * RATIO);

	// Clamp to max players
	{
		new diff = (fakesNum + playersNum) - (g_MaxPlayers - RESERVE_SLOTS) ;
		if(diff >= 0) {
			fakesNum = max(fakesNum -  diff, 0);
		}
	}
	
	new currentNumOfFakes = ArraySize(g_FakesArray);
	if(currentNumOfFakes > fakesNum) { // Add fakes
		RemoveFakes(currentNumOfFakes - fakesNum);
	} else if(fakesNum > currentNumOfFakes) { // Remove fakes
		AddFakes(fakesNum - currentNumOfFakes);
	}
}
