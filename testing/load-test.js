import http from 'k6/http';
import { htmlReport } from "https://raw.githubusercontent.com/benc-uk/k6-reporter/main/dist/bundle.js";

export const options = {
    insecureSkipTLSVerify: true,
    noConnectionReuse: false,
    scenarios: {
        constant_request_rate: {
          executor: 'constant-arrival-rate',
          rate: 20000,
          timeUnit: '1s', // 1000 iterations per second, i.e. 1000 RPS
          duration: '30s',
          preAllocatedVUs: 200, // how large the initial pool of VUs would be
          maxVUs: 200, // if the preAllocatedVUs are not enough, we can initialize more
        },
      },
  };

export default () => {
    http.get('http://127.0.0.1:8000/');
};

export function handleSummary(data) {
  return {
    "summary_mt.html": htmlReport(data),
  };
};