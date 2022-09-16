# frozen_string_literal: true

require 'net/http'
require 'uri'
require 'json'

def main
  trans = Trans.new(
    'https://procon33-practice.kosen.work',
    '834b206b66791fdae66002bd740d62699441babc29bb476bfdf0f9b73567e9ea'
  )

  case ARGV[0]
  when 'info'
    match_info_res = trans.match_info
    puts match_info_res.status
    puts match_info_res.pretty_read

    prob_info_res = trans.problem_info
    puts prob_info_res.status
    puts prob_info_res.pretty_read
  when 'get'
    return unless ARGV[1] =~ /\A[0-9]+\Z/

    chuncks_num = ARGV[1].to_i
    filenames_res = trans.get_filenames chuncks_num
    puts filenames_res.status
    trans.get_problems(chuncks_num)
  when 'post'
    probability_threshold = 30
    post_res = trans.submit(probability_threshold)
    puts post_res.status
    puts post_res.pretty_read
  end
end

class Trans
  def initialize(url, token)
    @uri = URI.parse url
    @header = { 'procon-token': token }
    @submit_header = { 'procon-token': token, 'Content-Type': 'application/json' }
    @http = Net::HTTP.new(@uri.host, @uri.port)
    @http.use_ssl = true
  end

  def match_info
    res = Response.new @http.request_get('/match', @header), Response::Type::MATCH_INFO
    return res unless res.success?

    File.write('./match_info.json', res.pretty_read)
    res
  end

  def problem_info
    Response.new @http.request_get('/problem', @header), Response::Type::PROBLEM_INFO
  end

  def get_filenames(chuncks_num)
    Response.new @http.post("/problem/chunks?n=#{chuncks_num.to_i}", nil, @header), Response::Type::FILENAMES
  end

  def get_problems(chuncks_num)
    prob_info_res = problem_info
    filenames_res = get_filenames(chuncks_num)
    return unless prob_info_res.success? and filenames_res.success?

    File.write('./problem_info.json', prob_info_res.pretty_read)

    filenames_res.filenames.each do |filename|
      res = Response.new @http.request_get('/problem/chunks/' + filename, @header), Response::Type::PROBLEM
      File.open(path + filename, 'wb') { |file| file.puts res.read }
    end
  end

  # @param [int] probability_threshold この確率未満の札は取らない(%表記)
  def submit(probability_threshold)
    subm = Trans.make_submit probability_threshold
    Response.new @http.post('/problem', subm, @submit_header), Response::Type::SUBMIT
  end

  def self.make_submit(probab_thresh)
    result = File.read('./result.txt')
    prob_info = JSON.parse(File.read('./problem_info.json'))
    probabs = result.split.map.with_index { |prob, idx| [prob.to_i, format('%02d', idx % 44 + 1)] }
    probabs.filter! { |prob, _| prob >= probab_thresh }.sort_by! { |prob, idx| [-prob, idx] }

    subm_hash = { 'problem-id': prob_info['id'], 'answers': probabs.map(&:last).uniq.take(prob_info['data']) }
    subm = JSON.pretty_generate subm_hash
    File.open("./history/results/#{prob_info['id']}.txt", 'w') { |file| file.puts result }
    File.open("./history/submits/#{prob_info['id']}.json", 'w') { |file| file.puts subm }
    subm
  end
end

class Response
  module Type
    MATCH_INFO = 0
    PROBLEM_INFO = 1
    FILENAMES = 2
    PROBLEM = 3
    SUBMIT = 4
  end

  def initialize(res, type)
    @res = res
    @type = type
  end

  def status
    "#{@res.code} [#{@res.message}] #{client_error? ? read : nil}"
  end

  def success?
    @res.is_a?(Net::HTTPSuccess)
  end

  def client_error?
    @res.is_a?(Net::HTTPClientError)
  end

  def type_of?(type)
    @type == type
  end

  def read
    @res.body
  end

  def parse
    return unless success?
    JSON.parse(read)
  end

  def pretty_read
    return unless success?
    JSON.pretty_generate(parse)
  end

  def filenames
    return unless success? and @type == Type::FILENAMES
    parse['chunks']
  end
end

main if __FILE__ == $PROGRAM_NAME
