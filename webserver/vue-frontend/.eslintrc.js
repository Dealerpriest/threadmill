module.exports = {
  root: true,

  env: {
    node: true
  },

  extends: ["plugin:vue/essential", "eslint:recommended"],

  rules: {
    "no-console": "off", // process.env.NODE_ENV === 'production' ? 'error' : 'off',
    "no-debugger": "off" //process.env.NODE_ENV === 'production' ? 'error' : 'off'
  },

  parserOptions: {
    parser: '@typescript-eslint/parser'
  },

  'extends': [
    'plugin:vue/essential',
    'eslint:recommended',
    '@vue/typescript'
  ]
};
