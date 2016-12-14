#!/usr/bin/env ruby

require 'listen'
require 'net/http'
require 'json'

class ThumperRestInterface

	@@DRIVE_SPEED = 70
	@@ID = 'Its dat boi'

	def initialize host='http://localhost:3000'
		@host = host
	end

	def strobe
		uri = URI(@host + '/neopixels/effects/strobe/0')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {red: 0, green: 240, blue: 0, delay: 50, id: @@ID }.to_json
		send_request uri, req
	end

	def strobe_blue
		uri = URI(@host + '/neopixels/effects/strobe/0')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {red: 0, green: 0, blue: 240, delay: 50, id: @@ID }.to_json
		send_request uri, req
	end

	def special_effect_2
		uri = URI(@host + '/neopixels/effects/shift/0')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {red: 125, green: 10, blue: 165, delay: 50, groupsize: 4, id: @@ID }.to_json
		send_request uri, req
	end


	def dim
		uri = URI(@host + '/neopixels/strings/0')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {red: 0, green: 0, blue: 0, id: @@ID }.to_json
		send_request uri, req
	end

	def triple_effect
		forward
		buzzer
		special_effect
	end

	def left
		drive @@DRIVE_SPEED, -@@DRIVE_SPEED
		strobe
	end

	def right
		drive -@@DRIVE_SPEED, @@DRIVE_SPEED
		special_effect
	end

	def forward
		drive @@DRIVE_SPEED, @@DRIVE_SPEED
		special_effect_2
	end

	def reverse
		drive -@@DRIVE_SPEED, -@@DRIVE_SPEED
		strobe_blue
	end

	def stop
		drive 0, 0
	end

	def buzzer
		uri = URI(@host + '/alarm')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {action: "toggle", id: @@ID }.to_json
		send_request uri, req
	end



	def special_effect
		uri = URI(@host + '/neopixels/effects/shift/0')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {red: 240, green: 0, blue: 0, delay: 50, groupsize: 8, id: @@ID }.to_json
		send_request uri, req
	end

	def drive leftspeed, rightspeed
		uri = URI(@host + '/speed')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {left_speed: leftspeed, right_speed: rightspeed, id: @@ID }.to_json
		send_request uri, req
	end

	def send_request uri, req
		res = Net::HTTP.start(uri.hostname, uri.port) do |http|
			http.request(req)
		end
	end
end

thumper = ThumperRestInterface.new "https://10.182.34.107:3000"

listener = Listen.to('/tmp/touch') do |modified|
  puts "modified absolute path: #{modified}"
	File.readlines(modified.first).each do |instruction|
		instruction.strip!

		if thumper.respond_to?(instruction.to_sym)
			thumper.send instruction
		else
			thumper.stop
		end

	end
end
listener.start # not blocking

sleep
