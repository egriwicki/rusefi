-- https://rusefi.com/forum/viewtopic.php?f=4&t=2325
-- tested on 9286699


CAN_BUTTONS = 0x267

brighness = 0

canRxAdd(0x264) -- rotation, I did not get it to work
canRxAdd(CAN_BUTTONS) -- buttons

setTickRate(5) -- set tick rate to 5hz

function onCanRx(bus, id, dlc, data)
	id = id % 2048
	-- 	print('got CAN id=' ..id ..' dlc=' ..dlc)

	if id == CAN_BUTTONS then
		-- 		print('got buttons @4=' ..data[4] ..' @6=' ..data[6])

		if data[4] == 1 or data[4] == 2 then
			print('got hold or release @4=' ..data[4] ..' @6=' ..data[6])

			if data[6] == 2 and brighness > 0 then
				brighness = brighness - 1
				print('brighness ' ..brighness)
			end

			if data[6] == 4 and brighness < 15 then
				brighness = brighness + 1
				print('brighness ' ..brighness)
			end
		end
	end
end

function onTick()
	-- wake up, need to send once every 1.5 second but we can send more often because why not
	txCan(1, 0x563, 0, { 3 })
	txCan(1, 0x202, 0, { brighness * 16 + 0xd })
end

