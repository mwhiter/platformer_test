#pragma once

// I'd like to read in data from files and NOT have to do this (alternatively, just fill up these arrays with the data from the files read in.
// But let's do it anyway for now. Maybe this can be a function of core. or something

#ifndef DATA_H
#define DATA_H

// todo: put this data in a separate file
// Weapon info
struct WeaponInfo
{
	WeaponInfo(std::string name, std::string sound, WeaponType eWeaponType, int rpm, int magsize, int maxammo, int projSpeed, int damage, int reloadSpeedMS) :
		sound(sound),
		name(name),
		weaponType(eWeaponType),
		rpm(rpm),
		magsize(magsize),
		maxammo(maxammo),
		projSpeed(projSpeed),
		damage(damage),
		reloadSpeedMS(reloadSpeedMS)
	{

	}

	std::string name;
	std::string sound;

	// stats
	WeaponType weaponType;
	int rpm;
	int magsize;
	int maxammo;
	int projSpeed;
	int damage;
	int reloadSpeedMS;
};

#endif